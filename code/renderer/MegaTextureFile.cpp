// MegaTextureFile.cpp
//

#include "Engine_precompiled.h"

#include "tr_local.h"

static union {
	int		intVal;
	byte	color[4];
} fillColor;

static byte	colors[8][4] = {
	{ 0, 0, 0, 255 },
	{ 255, 0, 0, 255 },
	{ 0, 255, 0, 255 },
	{ 255, 255, 0, 255 },
	{ 0, 0, 255, 255 },
	{ 255, 0, 255, 255 },
	{ 0, 255, 255, 255 },
	{ 255, 255, 255, 255 }
};

/*
===========================
rvmMegaTextureFile::rvmMegaTextureFile
===========================
*/
rvmMegaTextureFile::rvmMegaTextureFile()
{
	fileHandle = nullptr;
	numLevels = 0;
}

/*
===========================
rvmMegaTextureFile::~rvmMegaTextureFile
===========================
*/
rvmMegaTextureFile::~rvmMegaTextureFile() {
	if (fileHandle != nullptr) {
		fileSystem->CloseFile(fileHandle);
		fileHandle = nullptr;
	}
}

/*
===========================
rvmMegaTextureFile::LoadMegaTextureFile
===========================
*/
rvmMegaTextureFile *rvmMegaTextureFile::LoadMegaTextureFile(const char *name) {
	int		width, height;
	rvmMegaTextureFile *megaTextureFile = new rvmMegaTextureFile();
		
	megaTextureFile->fileHandle = fileSystem->OpenFileRead(name);
	if (!megaTextureFile->fileHandle) {
		common->Printf("rvmMegaTextureFile: failed to open %s\n", name);
		delete megaTextureFile;
		return nullptr;
	}

	megaTextureFile->fileHandle->Read(&megaTextureFile->header, sizeof(megaTextureFile->header));
	if (megaTextureFile->header.tileSize < 64 || megaTextureFile->header.tilesWide < 1 || megaTextureFile->header.tilesHigh < 1) {
		common->Printf("idMegaTexture: bad header on %s\n", name);
		delete megaTextureFile;
		return nullptr;
	}

	megaTextureFile->numLevels = 0;
	width = megaTextureFile->header.tilesWide;
	height = megaTextureFile->header.tilesHigh;

	int	tileOffset = 1;					// just past the header

	memset(megaTextureFile->levels, 0, sizeof(levels));
	while (1) {
		idTextureLevel *level = &megaTextureFile->levels[megaTextureFile->numLevels];

		level->mega = megaTextureFile;
		level->tileOffset = tileOffset;
		level->tilesWide = width;
		level->tilesHigh = height;
		level->parms[0] = -1;		// initially mask everything
		level->parms[1] = 0;
		level->parms[2] = 0;
		level->parms[3] = (float)width / (float)TILE_PER_LEVEL;
		level->Invalidate();

		tileOffset += level->tilesWide * level->tilesHigh;

		char	str[1024];
		sprintf(str, "_mega_%i", megaTextureFile->numLevels);

		// give each level a default fill color
		for (int i = 0; i < 4; i++) {
			fillColor.color[i] = colors[megaTextureFile->numLevels + 1][i];
		}
		// jmarshall - adapted to idTech 5 image code from BFG
		idImageOpts opts;
		opts.format = FMT_DXT5;
		opts.colorFormat = CFM_DEFAULT;
		opts.gammaMips = 0;
		opts.width = MAX_LEVEL_WIDTH;
		opts.height = MAX_LEVEL_WIDTH;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.numMSAASamples = 0;
		opts.numLevels = 1;

		idTempArray<byte> data(MAX_LEVEL_WIDTH * MAX_LEVEL_WIDTH * 4);

		for (int i = 0; i < MAX_LEVEL_WIDTH * MAX_LEVEL_WIDTH; i++) {
			((int *)data.Ptr())[i] = fillColor.intVal;
		}

		megaTextureFile->levels[megaTextureFile->numLevels].image = globalImages->ScratchImage(str, &opts, TF_LINEAR, TR_REPEAT, TD_DIFFUSE);
		megaTextureFile->levels[megaTextureFile->numLevels].image->UploadScratch(data.Ptr(), MAX_LEVEL_WIDTH, MAX_LEVEL_WIDTH);
		megaTextureFile->numLevels++;
		// jmarshall end		
		if (width <= TILE_PER_LEVEL && height <= TILE_PER_LEVEL) {
			break;
		}
		width = (width + 1) >> 1;
		height = (height + 1) >> 1;
	}

	return megaTextureFile;
}

/*
========================
rvmMegaTextureFile::ReadTile
========================
*/
void rvmMegaTextureFile::ReadTile(byte *tileBuffer, int tileNum) {
	int		tileSize = TILE_SIZE * TILE_SIZE;

	fileHandle->Seek(tileNum * tileSize, FS_SEEK_SET);
	//memset(data, 128, sizeof(data));
	fileHandle->Read(tileBuffer, tileSize);
}

/*
===========================
rvmMegaTextureFile::UpdateForCenter
===========================
*/
void rvmMegaTextureFile::UpdateForCenter(float texCenter[2]) {
	for (int i = 0; i < numLevels; i++) {
		levels[i].UpdateForCenter(texCenter);
	}
}
/*
===========================
rvmMegaTextureFile::Invalidate
===========================
*/
void rvmMegaTextureFile::Invalidate(void) {
	for (int i = 0; i < numLevels; i++) {
		levels[i].Invalidate();
	}
}

/*
===========================
rvmMegaTextureFile::BindForViewOrigin
===========================
*/
void rvmMegaTextureFile::BindForViewOrigin(const idVec3 viewOrigin) {
	// borderClamp image goes in texture 0
	GL_SelectTexture(0);
	globalImages->borderClampImage->Bind();

	// level images in higher textures, blurriest first
	for (int i = 0; i < 7; i++) {
		GL_SelectTexture(1 + i);

		if (i >= numLevels) {
			globalImages->whiteImage->Bind();

			static float	parms[4] = { -2, -2, 0, 1 };	// no contribution
			//glProgramLocalParameter4fvARB( GL_VERTEX_PROGRAM_ARB, i, parms );
			renderProgManager.SetUniformValue((const renderParm_t)(RENDERPARM_MEGALEVEL0 + i), parms);
		}
		else {
			idTextureLevel	*level = &levels[numLevels - 1 - i];

			if (idMegaTexture::r_showMegaTexture.GetBool()) {
				if (i & 1) {
					globalImages->blackImage->Bind();
				}
				else {
					globalImages->whiteImage->Bind();
				}
			}
			else {
				level->image->Bind();
			}
			//glProgramLocalParameter4fvARB( GL_VERTEX_PROGRAM_ARB, i, level->parms );
			renderProgManager.SetUniformValue((const renderParm_t)(RENDERPARM_MEGALEVEL0 + i), level->parms);
		}
	}

	float	parms[4];
	parms[0] = 0;
	parms[1] = 0;
	parms[2] = 0;
	parms[3] = 1;
	renderProgManager.SetUniformValue((const renderParm_t)(RENDERPARM_MEGALEVEL0 + 7), parms);
	//glProgramLocalParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 7, parms );

	//parms[0] = 1;
	//parms[1] = 1;
	//parms[2] = r_terrainScale.GetFloat();
	//parms[3] = 1;
	//glProgramLocalParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 8, parms );
}