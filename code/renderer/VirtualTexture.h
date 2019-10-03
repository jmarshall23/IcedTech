// VirtualTexture.h
//

#pragma once

// Version 1: Initial Implementation
// Version 2: Mipchain
#define VIRTUALTEXTURE_VERSION		2

#define VIRTUALTEXTURE_FILE			"virtualtextures/_virtualcache_v%d.pages"
#define VIRTUALTEXTURE_HEADER		"virtualtextures/_virtualcache_v%d.table"

#define VIRTUALTEXTURE_TILESIZE			128

#define MAXVIRTUALMATERIALS			1024

//
// rvmVirtualImage
//
class rvmVirtualImage {
public:
	rvmVirtualImage() {
		for (int i = 0; i < VIRTUALTEXTURE_MAXMIPS; i++)
		{
			pageOffset[i] = -1;
			width[i] = -1;
			height[i] = -1;
		}

		numMips = -1;
	}

	~rvmVirtualImage() {		
		for (int i = 0; i < VIRTUALTEXTURE_MAXMIPS; i++)
		{
			pageOffset[i] = -1;
			width[i] = -1;
			height[i] = -1;
		}
		numMips = -1;
	}

	const char *GetName() { return name.c_str(); }

	int		GetWidth(int mipIndex) { return width[mipIndex]; }
	int		GetHeight(int mipIndex) { return height[mipIndex]; }

	int		GetStartPageOffset(int mipIndex) { return pageOffset[mipIndex]; }

	int		GetWidthInPages(int mipIndex) { return width[mipIndex] / VIRTUALTEXTURE_TILESIZE; }
	int		GetHeightInPages(int mipIndex) { return height[mipIndex] / VIRTUALTEXTURE_TILESIZE; }

	int		GetNumMips() { return numMips; }

	textureUsage_t GetUsage() { return usage; }
protected:
	friend class rvmVirtualTextureSystem;

	idStr name;

	int width[VIRTUALTEXTURE_MAXMIPS];
	int height[VIRTUALTEXTURE_MAXMIPS];

	int numMips;
	int pageOffset[VIRTUALTEXTURE_MAXMIPS];
	textureUsage_t usage;
};

union feedbackPixel_t {
	int64_t packed;
	int16_t data[4];
};

//
// rvmVirtualTexturePage_t
//
struct rvmVirtualTexturePage_t {
	rvmVirtualTexturePage_t()
	{
		feedbackInfo.packed = 0;
		material = nullptr;
		uploadX = 0;
		uploadY = 0;
		evictionTime = 0;
	}

	int uploadX;
	int uploadY;
	feedbackPixel_t feedbackInfo;
	idMaterial *material;
	double	evictionTime;
};

//
// UnpackFeedbackPixel
//
ID_INLINE void UnpackFeedbackPixel(feedbackPixel_t &pixel, int &pageX, int &pageY, int &pageSource, int &pageLOD) {
	pageX = Float16ToFloat(pixel.data[0]);
	pageY = Float16ToFloat(pixel.data[1]);
	pageSource = Float16ToFloat(pixel.data[2]);
	pageLOD = Float16ToFloat(pixel.data[3]);
}

//
// rvmVirtualTextureSystem
//
class rvmVirtualTextureSystem {
public:
	// Starts up the virtual texture system.
	void					Init(void);

	// Loads in a virtual image.
	rvmVirtualImage			*LoadVirtualImage(const char *name, textureUsage_t usage, int desiredWidth, int desiredHeight);

	// Returns the virtual albedo texture.
	idImage					*GetVirtualAlbedoTexture();

	// Returns the virtual specular texture.
	idImage					*GetVirtualSpecularTexture();

	// Returns the virtual normal texture.
	idImage					*GetVirtualNormalTexture();

	// Stupid system to get around missing normal maps in Doom 3.
	rvmVirtualImage			*GetDefaultVirtualNormalTexture(int width, int height);

	// Processes the feedback job.
	void					RunFeedbackJob(idImage *feedbackImage);

	// Assign a material to the virtual material table.
	void					SetVirtualMaterial(int idx, idMaterial *material);
private:
	// Returns the name of the virtual texture file.
	const char				*GetVirtualTextureFileName();

	// Returns the name of the virtual texture header file.
	const char				*GetVirtualTextureHeaderFileName();
private:
	// Loads in a TGA.
	idFile *				LoadTGA(const char *name, TargaHeader &targa_header, int	&columns, int &rows, int &fileSize, int &numBytes);
	void					ProcessTGABlock(rvmMegaTextureSourceFile_t *file, byte *targa_rgba, TargaHeader	&targa_header, int columns, int rows, int scale);

	// Checks to see if the texture is uploaded, return -1 if it is, and if its not, returns a free page for us to upload too.
	bool					CheckFeedbackPixel(feedbackPixel_t pixel, int &uploadX, int &uploadY);

	// Transcodes the page to the image.
	void					TranscodePage(idImage *image, rvmVirtualImage *virtualImage, int pageX, int pageY, int uploadX, int uploadY, int pageLOD);

	// Generates a mipchain for a virtual image.
	void					GenerateVirtualImageMips(rvmVirtualImage *image);

	// Resize Image
	void					ResizeImage(const char *sourceImageName, const char *writeFileName, int width, int height);

	idFile					*virtualTextureFile;
	idFile					*virtualTextureWriter;

	//idFile					*virtualTextureHeader;
	idFile					*virtualTextureHeaderWriter;
private:
	idList<rvmVirtualImage *> virtualImages;
	
	rvmVirtualTexturePage_t *vt_pages;
	int	numVTPages;

	idImage					*albedoVirtualTexture;
	idImage					*specularVirtualTexture;
	idImage					*normalVirtualTexture;

	feedbackPixel_t			*feedbackCPUbuffer;


	byte					*transcodePage;
private:
	void					ClearVTPages(void);

	idMaterial				*virtualMaterials[MAXVIRTUALMATERIALS];

	rvmVirtualImage			*defaultSpecVirtualImage;
	idList<rvmVirtualImage *> defaultNormalVirtualImage;
private:
	byte					*transcodeFont;
	int						transcodeFontWidth;
	int						transcodeFontHeight;
	void					DrawTranscodeText(const char *msg, int x, int y, byte *transcodePage);
};

extern idCVar vt_ImageSize;
extern idCVar vt_transcodeShowPages;

extern rvmVirtualTextureSystem virtualTextureSystem;