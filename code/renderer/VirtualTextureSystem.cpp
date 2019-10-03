// VirtualTexture.cpp
//

#include "precompiled.h"

#include "tr_local.h"
#include "DXT/DXTCodec.h"
#include "Color/ColorSpace.h"

rvmVirtualTextureSystem virtualTextureSystem;

idCVar vt_ImageSize("vt_ImageSize", "8192", CVAR_INTEGER | CVAR_ROM, "size of the albedo/spec/normal virtual texture");

//
// ReadByte
//
static byte ReadByte(idFile *f) {
	byte	b;

	f->Read(&b, 1);
	return b;
}

static short ReadShort(idFile *f) {
	byte	b[2];

	f->Read(&b, 2);

	return b[0] + (b[1] << 8);
}

static int R_GetTargaBPP(TargaHeader &header) {
	switch (header.pixel_size)
	{
	case 8:
		return 1;

	case 24:
		return 3;

	case 32:
		return 4;
	}
	common->FatalError("Mega GetTargetBPP: Unknown BPP\n");
}

/*
=====================
rvmVirtualTextureSystem::Init
=====================
*/
void rvmVirtualTextureSystem::Init(void) {
	common->Printf("--------- VirtualVirtualSystem_Init ---------\n");

	// If we aren't in production, check if we are making new virtual texture files or if we are loading existing ones.
	if (!common->InProductionMode())
	{
		common->Printf("Virtual texture system is non production mode.\n");

		// Check to see if the virtual texture file exists, if not make a new one.
		if (fileSystem->FileExists(GetVirtualTextureFileName()) && fileSystem->FileExists(GetVirtualTextureHeaderFileName()))
		{
			common->Printf("Virtual texture file found...\n");
			virtualTextureWriter = fileSystem->OpenFileAppend(GetVirtualTextureFileName());
			virtualTextureHeaderWriter = fileSystem->OpenFileAppend(GetVirtualTextureHeaderFileName());
		}
		else
		{
			common->Printf("Virtual texture file not found, creating new one...\n");
			virtualTextureWriter = fileSystem->OpenFileWrite(GetVirtualTextureFileName());
			virtualTextureHeaderWriter = fileSystem->OpenFileWrite(GetVirtualTextureHeaderFileName());
		}

		// Set the virtual texture writers to the end of the file.
		virtualTextureHeaderWriter->Seek(virtualTextureHeaderWriter->Length(), FS_SEEK_SET);
		virtualTextureWriter->Seek(virtualTextureWriter->Length(), FS_SEEK_SET);
	}

	// Load in the virtual texture files.
	virtualTextureFile = fileSystem->OpenFileRead(GetVirtualTextureFileName());	
	
	idParser virtualTextureHeaderParser;
	virtualTextureHeaderParser.LoadFile(GetVirtualTextureHeaderFileName());
	virtualTextureHeaderParser.SetFlags(DECL_LEXER_FLAGS);

	// If we haven't loaded the virtual texture header or the virtual texture file, abort.
	if (!virtualTextureHeaderParser.IsLoaded() || virtualTextureFile == nullptr)
	{
		common->FatalError("Failed to load virtual texture file!");
		return;
	}

	// Parse the virtual texture header.
	while (!virtualTextureHeaderParser.EndOfFile())
	{
		idToken token;

		virtualTextureHeaderParser.ReadToken(&token);

		if (token.Length() == 0)
			continue;

		if (token == "texture") {
			rvmVirtualImage *virtualImage;

			virtualImage = new rvmVirtualImage();
			// texture %s %d %d %d\n", virtualImage->name.c_str(), virtualImage->width, virtualImage->height, virtualImage->pageOffset

			virtualTextureHeaderParser.ReadToken(&token);
			virtualImage->name = token;
			virtualImage->width[0] = virtualTextureHeaderParser.ParseInt();
			virtualImage->height[0] = virtualTextureHeaderParser.ParseInt();
			virtualImage->usage = (textureUsage_t)virtualTextureHeaderParser.ParseInt();
			virtualImage->numMips = virtualTextureHeaderParser.ParseInt();

			int width = virtualImage->width[0];
			int height = virtualImage->height[0];
			for (int i = 0; i < virtualImage->numMips; i++)
			{
				virtualImage->pageOffset[i] = virtualTextureHeaderParser.ParseInt();

				if (i > 0)
				{
					virtualImage->width[i] = (width + 1) >> 1;
					virtualImage->height[i] = (height + 1) >> 1;;

					width = virtualImage->width[i];
					height = virtualImage->height[i];
				}
			}
			virtualImages.Append(virtualImage);
		}
		else {
			common->FatalError("rvmVirtualTextureSystem::Init: Unknown or unexpected token %s\n", token.c_str());
		}
	}

	// Create the system virtual textures.
	{
		idImageOpts opts;
		opts.format = FMT_DXT5;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = vt_ImageSize.GetInteger();
		opts.height = vt_ImageSize.GetInteger();
		opts.numMSAASamples = 0;

		albedoVirtualTexture = renderSystem->CreateImage("_vt_albedo", &opts, TF_NEAREST);
		normalVirtualTexture = renderSystem->CreateImage("_vt_normal", &opts, TF_NEAREST);
		specularVirtualTexture = renderSystem->CreateImage("_vt_specular", &opts, TF_NEAREST);
	}

	numVTPages = vt_ImageSize.GetInteger() / VIRTUALTEXTURE_TILESIZE;

	vt_pages = new rvmVirtualTexturePage_t[numVTPages * numVTPages * 4];
	ClearVTPages();

	memset(virtualMaterials, 0, sizeof(virtualMaterials));

	transcodePage = new byte[VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE];

	feedbackCPUbuffer = nullptr;

	// Load the default no_spec and default no_normal virtual texture images.
	defaultNormalVirtualImage.Append(LoadVirtualImage("textures/engine/vt_default_normal", TD_BUMP, -1, -1));
	defaultNormalVirtualImage.Append(LoadVirtualImage("textures/engine/vt_default_normal_256", TD_BUMP, -1, -1));
	defaultNormalVirtualImage.Append(LoadVirtualImage("textures/engine/vt_default_normal256_128", TD_BUMP, -1, -1));
	defaultSpecVirtualImage = LoadVirtualImage("textures/engine/vt_default_spec", TD_SPECULAR, -1, -1);
	if (defaultSpecVirtualImage == nullptr) {
		common->FatalError("Failed to load default virtual images!");
	}

	// Load in the transcode font.
	R_LoadTGA("textures/engine/vt_transcode_font.tga", &transcodeFont, &transcodeFontWidth, &transcodeFontHeight, nullptr);
	if (transcodeFont == nullptr) {
		common->FatalError("Failed to load the transcode font!");
	}

	// Prevent adjustments happening on the first frame. 
	vt_transcodeShowPages.ClearModified();

	common->Printf("Loaded %d virtual textures\n", virtualImages.Num());
}

/*
====================
rvmVirtualTextureSystem::ClearVTPages
====================
*/
void rvmVirtualTextureSystem::ClearVTPages(void) {
	memset(&vt_pages[0], 0, numVTPages * numVTPages * 4 * sizeof(rvmVirtualTexturePage_t));

	for (int y = 0; y < numVTPages; y++)
	{
		for (int x = 0; x < numVTPages; x++)
		{
			vt_pages[(y * numVTPages) + x].uploadX = x;
			vt_pages[(y * numVTPages) + x].uploadY = y;
		}
	}
}

/*
====================
rvmVirtualTextureSystem::LoadTGA
====================
*/
idFile *rvmVirtualTextureSystem::LoadTGA(const char *name, TargaHeader &targa_header, int	&columns, int &rows, int &fileSize, int &numBytes)
{
	//
	// open the file
	//
	common->Printf("Opening %s.\n", name);
	fileSize = fileSystem->ReadFile(name, NULL, NULL);
	idFile	*file = fileSystem->OpenFileRead(name);

	if (!file) {
		common->Printf("Couldn't open %s\n", name);
		return nullptr;
	}

	targa_header.id_length = ReadByte(file);
	targa_header.colormap_type = ReadByte(file);
	targa_header.image_type = ReadByte(file);

	targa_header.colormap_index = ReadShort(file);
	targa_header.colormap_length = ReadShort(file);
	targa_header.colormap_size = ReadByte(file);
	targa_header.x_origin = ReadShort(file);
	targa_header.y_origin = ReadShort(file);
	targa_header.width = ReadShort(file);
	targa_header.height = ReadShort(file);
	targa_header.pixel_size = ReadByte(file);
	targa_header.attributes = ReadByte(file);

	if (targa_header.image_type != 2 && targa_header.image_type != 10 && targa_header.image_type != 3) {
		common->Error("LoadTGA( %s ): Only type 2 (RGB), 3 (gray), and 10 (RGB) TGA images supported\n", name);
	}

	if (targa_header.colormap_type != 0) {
		common->Error("LoadTGA( %s ): colormaps not supported\n", name);
	}

	if ((targa_header.pixel_size != 32 && targa_header.pixel_size != 24) && targa_header.image_type != 3) {
		common->Error("LoadTGA( %s ): Only 32 or 24 bit images supported (no colormaps)\n", name);
	}

	//if (targa_header.image_type == 2 || targa_header.image_type == 3) {
	//	numBytes = targa_header.width * targa_header.height * (targa_header.pixel_size >> 3);
	//	if (numBytes > fileSize - 18 - targa_header.id_length) {
	//		common->Error("LoadTGA( %s ): incomplete file\n", name);
	//	}
	//}

	columns = targa_header.width;
	rows = targa_header.height;

	// skip TARGA image comment
	if (targa_header.id_length != 0) {
		file->Seek(targa_header.id_length, FS_SEEK_CUR);
	}

	return file;
}


/*
====================
rvmVirtualTextureSystem::ProcessTGABlock
====================
*/
void rvmVirtualTextureSystem::ProcessTGABlock(rvmMegaTextureSourceFile_t *file, byte *targa_rgba, TargaHeader	&targa_header, int columns, int rows, int scale)
{
	int		row, column;
	byte	*pixbuf;
	byte	*startRowPixBuf;

	if (targa_header.image_type == 2 || targa_header.image_type == 3) {
		// Uncompressed RGB or gray scale image
		for (row = 0; row < VIRTUALTEXTURE_TILESIZE * scale; row++) {
			pixbuf = targa_rgba + row * (columns * scale) * 4;
			startRowPixBuf = pixbuf;

			for (column = 0; column < columns; column++) {
				unsigned char red, green, blue, alphabyte;
				switch (targa_header.pixel_size) {
				case 8:
					blue = file->ReadByte();
					green = blue;
					red = blue;

					for (int f = 0; f < scale; f++)
					{
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = 255;
					}
					break;

				case 24:
					blue = file->ReadByte();
					green = file->ReadByte();
					red = file->ReadByte();
					for (int f = 0; f < scale; f++)
					{
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = 255;
					}
					break;
				case 32:
					blue = file->ReadByte();
					green = file->ReadByte();
					red = file->ReadByte();
					alphabyte = file->ReadByte();
					for (int f = 0; f < scale; f++)
					{
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
					}
					break;
				default:
					common->Error("LoadTGA: illegal pixel_size '%d'\n", targa_header.pixel_size);
					break;
				}
			}

			// If we are scaling, just duplicate the row!
			int scale_loop = scale;
			while (scale_loop > 1)
			{
				int64_t rowlength = pixbuf - startRowPixBuf;

				memcpy(pixbuf, startRowPixBuf, rowlength);
				pixbuf += rowlength;

				row++;
				scale_loop--;
			}
		}
	}
	else if (targa_header.image_type == 10) {   // Runlength encoded RGB images
		// jmarshall: I'm not supporting RLE so we can pre-load as much as the image as possible off disc during baking.
		common->FatalError("MegaTexture ProcessTGABlock: RLE not supported!");
	}
}


/*
====================
rvmVirtualTextureSystem::GenerateVirtualImageMips
====================
*/
void rvmVirtualTextureSystem::GenerateVirtualImageMips(rvmVirtualImage *image) {
	virtualTextureWriter->Flush();

	int	width = image->GetWidthInPages(0);
	int	height = image->GetHeightInPages(0);
	int pageMemSize = VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE;

	int		tileSize = VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE * 4;
	int		tileSizeCompressed = VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE;
	byte	*oldBlock = (byte *)_alloca(tileSize);
	byte	*oldBlockCompressed = (byte *)_alloca(tileSizeCompressed);
	byte	*newBlock = (byte *)_alloca(tileSize);
	byte	*newBlockCompressed = (byte *)_alloca(tileSizeCompressed);

	image->numMips = 1;

	int imageWidth = image->GetWidth(0);
	int imageHeight = image->GetHeight(0);

	while (imageWidth > VIRTUALTEXTURE_TILESIZE && imageHeight > VIRTUALTEXTURE_TILESIZE) {
		int	newHeight = (height + 1) >> 1;
		if (newHeight < 1) {
			break;
		}
		int	newWidth = (width + 1) >> 1;
		if (newWidth < 1) {
			break;
		}

		imageWidth = imageWidth >> 1;
		imageHeight = imageHeight >> 1;

		//common->Printf("generating %i x %i block mip level\n", newWidth, newHeight);
		image->pageOffset[image->numMips] = virtualTextureWriter->Tell();
		for (int y = 0; y < newHeight; y++) {
			//common->Printf("row %i\n", y);
			//session->UpdateScreen();

			for (int x = 0; x < newWidth; x++) {
				// mip map four original blocks down into a single new block
				for (int yy = 0; yy < 2; yy++) {
					for (int xx = 0; xx < 2; xx++) {
						int	tx = x * 2 + xx;
						int ty = y * 2 + yy;

						if (tx > width || ty > height) {
							// off edge, zero fill
							memset(newBlock, 0, sizeof(newBlock));
						}
						else {
							int tilePagePosition = image->GetStartPageOffset(image->numMips - 1) + (pageMemSize * ty * image->GetWidthInPages(image->numMips - 1)) + (pageMemSize * tx);

							virtualTextureFile->Seek(tilePagePosition, FS_SEEK_SET);
							virtualTextureFile->Read(oldBlockCompressed, tileSizeCompressed);

							idDxtDecoder decoder;
							decoder.DecompressImageDXT5(oldBlockCompressed, oldBlock, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
							//idColorSpace::ConvertCoCg_YToRGB(oldBlockCompressed, oldBlockCompressed, tileSize, tileSize);
						//	R_WriteTGA("test_original.tga", (const byte *)oldBlock, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
						}
						// mip map the new pixels
						for (int yyy = 0; yyy < VIRTUALTEXTURE_TILESIZE / 2; yyy++) {
							for (int xxx = 0; xxx < VIRTUALTEXTURE_TILESIZE / 2; xxx++) {
								byte *in = &oldBlock[(yyy * 2 * VIRTUALTEXTURE_TILESIZE + xxx * 2) * 4];
								byte *out = &newBlock[(((VIRTUALTEXTURE_TILESIZE / 2 * yy) + yyy) * VIRTUALTEXTURE_TILESIZE + (VIRTUALTEXTURE_TILESIZE / 2 * xx) + xxx) * 4];
								out[0] = (in[0] + in[4] + in[0 + VIRTUALTEXTURE_TILESIZE * 4] + in[4 + VIRTUALTEXTURE_TILESIZE * 4]) >> 2;
								out[1] = (in[1] + in[5] + in[1 + VIRTUALTEXTURE_TILESIZE * 4] + in[5 + VIRTUALTEXTURE_TILESIZE * 4]) >> 2;
								out[2] = (in[2] + in[6] + in[2 + VIRTUALTEXTURE_TILESIZE * 4] + in[6 + VIRTUALTEXTURE_TILESIZE * 4]) >> 2;
								out[3] = (in[3] + in[7] + in[3 + VIRTUALTEXTURE_TILESIZE * 4] + in[7 + VIRTUALTEXTURE_TILESIZE * 4]) >> 2;
							}
						}					
					}
				}

				idDxtEncoder encoder;

				//idColorSpace::ConvertRGBToCoCg_Y(newBlock, newBlock, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
				encoder.CompressImageDXT5Fast((const byte *)newBlock, newBlockCompressed, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
				//outFile->Seek(tileNum * tileSizeCompressed, FS_SEEK_SET);										
				virtualTextureWriter->Write(newBlockCompressed, tileSizeCompressed);
			}
		}
		width = newWidth;
		height = newHeight;
		
		image->width[image->numMips] = imageWidth;
		image->height[image->numMips] = imageHeight;
		image->numMips++;	
	}
}

/*
=====================
rvmVirtualTextureSystem::ResizeImage
=====================
*/
void rvmVirtualTextureSystem::ResizeImage(const char *sourceImageName, const char *writeFileName, int scaled_width, int scaled_height)
{
	byte *newImageBuffer;
	byte *sourceImageData;
	int sourceWidth;
	int sourceHeight;

	// Load the entire source into memory.
	R_LoadTGA(sourceImageName, &sourceImageData, &sourceWidth, &sourceHeight, NULL);

	// Resample the texture.
	newImageBuffer = R_Dropsample(sourceImageData, sourceWidth, sourceHeight, scaled_width, scaled_height);

	// Write out the generated targa.
	R_WriteTGA(writeFileName, newImageBuffer, scaled_width, scaled_height);

	R_StaticFree(sourceImageData);
	R_StaticFree(newImageBuffer);
}

/*
=====================
rvmVirtualTextureSystem::LoadVirtualImage
=====================
*/
rvmVirtualImage	*rvmVirtualTextureSystem::LoadVirtualImage(const char *name, textureUsage_t usage, int desiredWidth, int desiredHeight) {
	int currentSourceTilePosition;

	// See if we already have a virtual image for this texture.
	for (int i = 0; i < virtualImages.Num(); i++)
	{
		if (!strcmp(virtualImages[i]->GetName(), name)) {
			return virtualImages[i];
		}
	}

	idStr fileName;

	// Check to see if this image needs to be resized.
	// Doom 3 base assets and even the HRP aren't power of two and are weird sizes.
	// With virtual texturing, we need all material textures to be the same size and power of two, fix it here it need be.
	{
		fileName = name;
		fileName.SetFileExtension("tga");

		rvmMegaTextureSourceFile_t imageTestSource;
		imageTestSource.file = rvmVirtualTextureSystem::LoadTGA(fileName.c_str(), imageTestSource.targa_header, imageTestSource.columns, imageTestSource.rows, imageTestSource.fileSize, imageTestSource.numBytes);

		if (imageTestSource.file == nullptr)
		{
			common->Warning("rvmVirtualTextureSystem::LoadVirtualImage: Failed to load image %s", name);
			return nullptr;
		}

		bool needsRLEConversion = imageTestSource.targa_header.image_type == 10;

		// If we don't have a desired size, lets just assume this texture needs to be power of 2, and test for that.
		if (desiredWidth != -1 && desiredHeight != -1)
		{
			desiredWidth = RoundDownToPowerOfTwo(desiredWidth);
			desiredHeight = RoundDownToPowerOfTwo(desiredHeight);
		}
		else
		{
			desiredWidth = RoundDownToPowerOfTwo(imageTestSource.columns);
			desiredHeight = RoundDownToPowerOfTwo(imageTestSource.rows);
		}

		// Ensure the textures are big enough to be virtualtextured(Doom 3 had some very small assets).
		while (desiredWidth < VIRTUALTEXTURE_TILESIZE || desiredHeight < VIRTUALTEXTURE_TILESIZE)
		{
			desiredWidth = desiredWidth * 2;
			desiredHeight = desiredHeight * 2;
		}

		// Check if it needs to be resized.
		if (desiredWidth != imageTestSource.columns || desiredHeight != imageTestSource.rows || needsRLEConversion)
		{
			common->Printf("Resizing image %s because it doesn't mean the requirements for virtual texturing, consider fixing this asset!\n", name);
			idStr newFileName;
			
			if (usage == TD_BUMP)
			{
				newFileName = fileName;
				newFileName.SetFileExtension(".resampled.tga");
			}
			else
			{
				newFileName = va("generated/image_scaling/%s.resampled.tga", name);
			}

			// Destroy imageTestSource, its no longer needed.
			imageTestSource.Close();

			// Resize the image.
			ResizeImage(fileName.c_str(), newFileName.c_str(), desiredWidth, desiredHeight);
			fileName = newFileName;
		}
	}

	// Allocate a new virtual image for this image.
	rvmMegaTextureSourceFile_t imageSource;
	rvmVirtualImage *virtualImage;

	virtualImage = new rvmVirtualImage();

	imageSource.file = rvmVirtualTextureSystem::LoadTGA(fileName.c_str(), imageSource.targa_header, imageSource.columns, imageSource.rows, imageSource.fileSize, imageSource.numBytes);
	if (imageSource.file == nullptr) {
		common->Warning("rvmVirtualTextureSystem::LoadVirtualImage: Failed to load image %s", name);
		return nullptr;
	}

	if (imageSource.columns < VIRTUALTEXTURE_TILESIZE || imageSource.rows < VIRTUALTEXTURE_TILESIZE) {
		common->Warning("rvmVirtualTextureSystem::LoadVirtualImage: Image size for %s too small", name);
		return nullptr;
	}

	int imageSourceTileSize = (VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE + 1) * 4;
	byte *imageSourceTileScratch = new byte[imageSourceTileSize];
	byte *compressed_tile_buffer = new byte[imageSourceTileSize];
	byte *targa_rgba = new byte[VIRTUALTEXTURE_TILESIZE * imageSource.targa_header.width * 4];

	// Construct the virtual image header.
	virtualImage->name = name;
	virtualImage->width[0] = imageSource.columns;
	virtualImage->height[0] = imageSource.rows;
	virtualImage->usage = usage;
	virtualImage->numMips = 1;
	virtualImage->pageOffset[0] = virtualTextureWriter->Tell();

	// We need to extract TILE_SIZE x TILE_SIZE blocks of this texture, generate a mipmap chain for those blocks, and save it to the pages file.
	int tilesWide = virtualImage->width[0] / VIRTUALTEXTURE_TILESIZE;
	int tilesHigh = virtualImage->height[0] / VIRTUALTEXTURE_TILESIZE;

	common->Printf("Building virtual texture %s (%dx%d tiles)...\n", name, tilesWide, tilesHigh);

	// Grab some information about this image.
	int imageSourcebpp = R_GetTargaBPP(imageSource.targa_header);
	int imageSourceLen = (imageSource.columns * imageSourcebpp);
	imageSourceLen = imageSourceLen * VIRTUALTEXTURE_TILESIZE;

	imageSource.AllocScratch(imageSourceLen);

	// Process each block.
	int blockRowsRemaining = tilesHigh;
	while (blockRowsRemaining--) {
		//common->Printf("%i blockRowsRemaining\n", blockRowsRemaining);
		//session->UpdateScreen();

		// Read in the block(TILESIZE * image_width).
		imageSource.file->Read(imageSource.scratch, imageSourceLen);

		// Process the image source.
		imageSource.ResetScratch();
		ProcessTGABlock(&imageSource, targa_rgba, imageSource.targa_header, imageSource.columns, imageSource.rows, 1);

		//
		// write out individual blocks from the full row block buffer
		//
		for (int rowBlock = 0; rowBlock < tilesWide; rowBlock++) {
			idDxtEncoder encoder;

			//megaMemoryTile->Seek(0, FS_SEEK_SET);
			currentSourceTilePosition = 0;
			for (int y = 0; y < VIRTUALTEXTURE_TILESIZE; y++) {
				memcpy(&imageSourceTileScratch[currentSourceTilePosition], targa_rgba + (y * imageSource.targa_header.width + rowBlock * VIRTUALTEXTURE_TILESIZE) * 4, VIRTUALTEXTURE_TILESIZE * 4);
				currentSourceTilePosition += VIRTUALTEXTURE_TILESIZE * 4;
			}

			// convert the image data to YCoCg and use the YCoCgDXT5 compressor
			if (usage == TD_BUMP)
			{
				for (int i = 0; i < VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE; i++) {
					imageSourceTileScratch[i * 4 + 3] = imageSourceTileScratch[i * 4 + 0];
					imageSourceTileScratch[i * 4 + 0] = 0;
					imageSourceTileScratch[i * 4 + 2] = 0;
				}
				encoder.CompressNormalMapDXT5Fast((const byte *)imageSourceTileScratch, compressed_tile_buffer, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
			}
			else
			{
				//idColorSpace::ConvertRGBToCoCg_Y((byte *)imageSourceTileScratch, (byte *)imageSourceTileScratch, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
				encoder.CompressImageDXT5Fast((const byte *)imageSourceTileScratch, compressed_tile_buffer, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
			}

			// Write out the page to the virtual texture file.
			virtualTextureWriter->Write(compressed_tile_buffer, VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE);
		}
	}

	virtualTextureWriter->Flush();
	virtualTextureHeaderWriter->Flush();

	delete imageSourceTileScratch;
	delete compressed_tile_buffer;
	delete targa_rgba;

	// Construct the mipchain.
	GenerateVirtualImageMips(virtualImage);

	// Write out the virtual image header entry.
	idStr vtHeaderEntry = va("texture %s %d %d %d %d", virtualImage->name.c_str(), virtualImage->width[0], virtualImage->height[0], virtualImage->usage, virtualImage->numMips);
	for (int i = 0; i < virtualImage->numMips; i++)
	{
		vtHeaderEntry += va(" %d", virtualImage->pageOffset[i]);
	}
	vtHeaderEntry += va("\n");
	virtualTextureHeaderWriter->Write(vtHeaderEntry.c_str(), vtHeaderEntry.Length());

	// Add the virtual texture to the list and return it.
	virtualImages.Append(virtualImage);
	return virtualImage;
}

/*
=====================
rvmVirtualTextureSystem::GetVirtualTextureFileName
=====================
*/
const char * rvmVirtualTextureSystem::GetVirtualTextureFileName() {
	static idStr fileName;
	fileName = va(VIRTUALTEXTURE_FILE, VIRTUALTEXTURE_VERSION);
	return fileName;
}

/*
=====================
rvmVirtualTextureSystem::GetVirtualTextureHeaderFileName
=====================
*/
const char * rvmVirtualTextureSystem::GetVirtualTextureHeaderFileName() {
	static idStr fileName;
	fileName = va(VIRTUALTEXTURE_HEADER, VIRTUALTEXTURE_VERSION);
	return fileName;
}

/*
=============================
rvmVirtualTextureSystem::GetVirtualAlbedoTexture
=============================
*/
idImage	*rvmVirtualTextureSystem::GetVirtualAlbedoTexture() {
	return albedoVirtualTexture;
}

/*
=============================
rvmVirtualTextureSystem::GetVirtualSpecularTexture
=============================
*/
idImage	*rvmVirtualTextureSystem::GetVirtualSpecularTexture() {
	return specularVirtualTexture;
}

/*
=============================
rvmVirtualTextureSystem::GetVirtualSpecularTexture
=============================
*/
idImage	*rvmVirtualTextureSystem::GetVirtualNormalTexture() {
	return normalVirtualTexture;
}

/*
=============================
rvmVirtualTextureSystem::SetVirtualMaterial
=============================
*/
void rvmVirtualTextureSystem::SetVirtualMaterial(int idx, idMaterial *material) {
	virtualMaterials[idx] = material;
}

/*
=============================
rvmVirtualTextureSystem::GetDefaultVirtualNormalTexture
=============================
*/
rvmVirtualImage	*rvmVirtualTextureSystem::GetDefaultVirtualNormalTexture(int width, int height) {
	rvmVirtualImage *defaultNormalImage = nullptr;

	for (int d = 0; d < defaultNormalVirtualImage.Num(); d++)
	{
		if (width == defaultNormalVirtualImage[d]->GetWidth(0) && height == defaultNormalVirtualImage[d]->GetHeight(0))
		{
			defaultNormalImage = defaultNormalVirtualImage[d];
			break;
		}
	}

	if (defaultNormalImage == nullptr)
	{
		common->Warning("Material is missing a normal map, and failed to find a default fallback(%dx%d)! Just add a normal here.", width, height);
		return defaultNormalVirtualImage[0];
	}

	return defaultNormalImage;
}