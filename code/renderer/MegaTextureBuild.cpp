// MegaTextureBuild.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"
#include "DXT/DXTCodec.h"
#include "Color/ColorSpace.h"

idCVar idMegaTexture::r_megatexture_ambient("r_megatexture_ambient", "20", CVAR_RENDERER | CVAR_INTEGER, "amount of lighting to add to the lit megatexture during building");

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

int RoundDownToPowerOfTwo(int num) {
	int		pot;
	for (pot = 1; (pot * 2) <= num; pot <<= 1) {
	}
	return pot;
}

int R_GetTargaBPP(TargaHeader &header) {
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
====================
GenerateMegaMipMaps
====================
*/
void	idMegaTexture::GenerateMegaMipMaps(megaTextureHeader_t *header, idFile *outFile) {
	outFile->Flush();

	// out fileSystem doesn't allow read / write access...
	idFile	*inFile = fileSystem->OpenFileRead(outFile->GetName());

	int	tileOffset = 1;
	int	width = header->tilesWide;
	int	height = header->tilesHigh;

	int		tileSize = header->tileSize * header->tileSize * 4;
	int		tileSizeCompressed = header->tileSize * header->tileSize;
	byte	*oldBlock = (byte *)_alloca(tileSize);
	byte	*oldBlockCompressed = (byte *)_alloca(tileSizeCompressed);
	byte	*newBlock = (byte *)_alloca(tileSize);
	byte	*newBlockCompressed = (byte *)_alloca(tileSizeCompressed);

	while (width > 1 || height > 1) {
		int	newHeight = (height + 1) >> 1;
		if (newHeight < 1) {
			newHeight = 1;
		}
		int	newWidth = (width + 1) >> 1;
		if (width < 1) {
			width = 1;
		}
		common->Printf("generating %i x %i block mip level\n", newWidth, newHeight);

		int		tileNum;

		for (int y = 0; y < newHeight; y++) {
			common->Printf("row %i\n", y);
			session->UpdateScreen();

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
							tileNum = tileOffset + ty * width + tx;
							inFile->Seek(tileNum * tileSizeCompressed, FS_SEEK_SET);
							inFile->Read(oldBlockCompressed, tileSizeCompressed);

							idDxtDecoder decoder;
							decoder.DecompressYCoCgDXT5(oldBlockCompressed, oldBlock, TILE_SIZE, TILE_SIZE);
							//idColorSpace::ConvertCoCg_YToRGB(oldBlockCompressed, oldBlockCompressed, tileSize, tileSize);

						//	R_WriteTGA("test.tga", (const byte *)oldBlock, TILE_SIZE, TILE_SIZE);
						}
						// mip map the new pixels
						for (int yyy = 0; yyy < TILE_SIZE / 2; yyy++) {
							for (int xxx = 0; xxx < TILE_SIZE / 2; xxx++) {
								byte *in = &oldBlock[(yyy * 2 * TILE_SIZE + xxx * 2) * 4];
								byte *out = &newBlock[(((TILE_SIZE / 2 * yy) + yyy) * TILE_SIZE + (TILE_SIZE / 2 * xx) + xxx) * 4];
								out[0] = (in[0] + in[4] + in[0 + TILE_SIZE * 4] + in[4 + TILE_SIZE * 4]) >> 2;
								out[1] = (in[1] + in[5] + in[1 + TILE_SIZE * 4] + in[5 + TILE_SIZE * 4]) >> 2;
								out[2] = (in[2] + in[6] + in[2 + TILE_SIZE * 4] + in[6 + TILE_SIZE * 4]) >> 2;
								out[3] = (in[3] + in[7] + in[3 + TILE_SIZE * 4] + in[7 + TILE_SIZE * 4]) >> 2;
							}
						}

						// write the block out
						tileNum = tileOffset + width * height + y * newWidth + x;


						idDxtEncoder encoder;
						//idColorSpace::ConvertRGBToCoCg_Y(newBlock, newBlock, TILE_SIZE, TILE_SIZE);
						encoder.CompressYCoCgDXT5Fast((const byte *)newBlock, newBlockCompressed, TILE_SIZE, TILE_SIZE);
						outFile->Seek(tileNum * tileSizeCompressed, FS_SEEK_SET);
						outFile->Write(newBlockCompressed, tileSizeCompressed);
					}
				}
			}
		}
		tileOffset += width * height;
		width = newWidth;
		height = newHeight;
	}

	delete inFile;
}

/*
====================
GenerateMegaPreview

Make a 2k x 2k preview image for a mega texture that can be used in modeling programs
====================
*/
void	idMegaTexture::GenerateMegaPreview(const char *fileName) {
	idFile	*fileHandle = fileSystem->OpenFileRead(fileName);
	if (!fileHandle) {
		common->Printf("idMegaTexture: failed to open %s\n", fileName);
		return;
	}

	idStr	outName = fileName;
	outName.StripFileExtension();
	outName += "_preview.tga";

	common->Printf("Creating %s.\n", outName.c_str());

	megaTextureHeader_t header;

	fileHandle->Read(&header, sizeof(header));
	if (header.tileSize < 64 || header.tilesWide < 1 || header.tilesHigh < 1) {
		common->Printf("idMegaTexture: bad header on %s\n", fileName);
		return;
	}

	int	tileSize = header.tileSize;
	int	width = header.tilesWide;
	int	height = header.tilesHigh;
	int	tileOffset = 1;
	int	tileBytes = tileSize * tileSize;
	// find the level that fits
	while (width * tileSize > 2048 || height * tileSize > 2048) {
		tileOffset += width * height;
		width >>= 1;
		if (width < 1) {
			width = 1;
		}
		height >>= 1;
		if (height < 1) {
			height = 1;
		}
	}

	byte *pic = (byte *)R_StaticAlloc(width * height * (tileBytes * 4));
	byte	*oldBlock = (byte *)_alloca(tileBytes);
	byte *oldBlockUncompressed = (byte *)R_StaticAlloc(tileSize * tileSize * 4);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int tileNum = tileOffset + y * width + x;
			fileHandle->Seek(tileNum * tileBytes, FS_SEEK_SET);
			fileHandle->Read(oldBlock, tileBytes);

			idDxtDecoder decoder;
			decoder.DecompressYCoCgDXT5(oldBlock, oldBlockUncompressed, tileSize, tileSize);
			idColorSpace::ConvertCoCg_YToRGB(oldBlockUncompressed, oldBlockUncompressed, tileSize, tileSize);

			for (int yy = 0; yy < tileSize; yy++) {
				memcpy(pic + ((y * tileSize + yy) * width * tileSize + x * tileSize) * 4,
					oldBlockUncompressed + yy * tileSize * 4, tileSize * 4);
			}
		}
	}



	R_WriteTGA(outName.c_str(), pic, width * tileSize, height * tileSize, false);

	R_StaticFree(oldBlockUncompressed);
	R_StaticFree(pic);

	delete fileHandle;
}

/*
====================
idMegaTexture::LoadTGA
====================
*/
idFile *idMegaTexture::LoadTGA(const char *name, TargaHeader &targa_header, int	&columns, int &rows, int &fileSize, int &numBytes)
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
idMegaTexture::ProcessTGABlock
====================
*/
void idMegaTexture::ProcessTGABlock(rvmMegaTextureSourceFile_t *file, byte *targa_rgba, TargaHeader	&targa_header, int columns, int rows, int scale)
{
	int		row, column;
	byte	*pixbuf;
	byte	*startRowPixBuf;
	
	if (targa_header.image_type == 2 || targa_header.image_type == 3) {
		// Uncompressed RGB or gray scale image
		for (row = 0; row < TILE_SIZE * scale; row++) {
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
MakeMegaTexture_f

Incrementally load a giant tga file and process into the mega texture block format
====================
*/
void idMegaTexture::MakeMegaTexture_f(const idCmdArgs &args) {
	rvmMegaTextureSourceFile_t albedoSource, litSource;

	if (args.Argc() != 2) {
		common->Printf("USAGE: makeMegaTexture <filebase>\n");
		return;
	}

	idStr	name = "megaTextures/";
	name += args.Argv(1);
	name.StripFileExtension();
	name += ".tga";

	idStr	albedoName = "megagen/bin/";
	albedoName += args.Argv(1);
	albedoName.StripFileExtension();
	albedoName += ".tga";

	idStr	lit_name = "megaTextures/";
	lit_name += args.Argv(1);
	lit_name.StripFileExtension();
	lit_name += "_lit.tga";

	albedoSource.file = idMegaTexture::LoadTGA(albedoName, albedoSource.targa_header, albedoSource.columns, albedoSource.rows, albedoSource.fileSize, albedoSource.numBytes);
	if (albedoSource.file == nullptr)
		return;

	litSource.file = idMegaTexture::LoadTGA(lit_name, litSource.targa_header, litSource.columns, litSource.rows, litSource.fileSize, litSource.numBytes);
	if (litSource.file == nullptr)
		return;

	megaTextureHeader_t		mtHeader;

	mtHeader.tileSize = TILE_SIZE;
	mtHeader.tilesWide = RoundDownToPowerOfTwo(albedoSource.targa_header.width) / TILE_SIZE;
	mtHeader.tilesHigh = RoundDownToPowerOfTwo(albedoSource.targa_header.height) / TILE_SIZE;

	idStr	outName = name;
	outName.StripFileExtension();
	outName += ".mega";

	common->Printf("Writing %i x %i size %i tiles to %s.\n", mtHeader.tilesWide, mtHeader.tilesHigh, mtHeader.tileSize, outName.c_str());

	// open the output megatexture file
	idFile	*out = fileSystem->OpenFileWrite(outName.c_str());

	out->Write(&mtHeader, sizeof(mtHeader));
	out->Seek(TILE_SIZE * TILE_SIZE, FS_SEEK_SET);

	// we will process this one row of tiles at a time, since the entire thing
	// won't fit in memory
	byte	*targa_rgba = (byte *)R_StaticAlloc(TILE_SIZE * albedoSource.targa_header.width * 4);	
	byte	*targa_compose = (byte *)R_StaticAlloc(TILE_SIZE * albedoSource.targa_header.width * 4);

	int len = (TILE_SIZE * TILE_SIZE + 1) * 4;
	byte *megaMemory = (byte *)Mem_Alloc(len);
	//idFile_Memory *megaMemoryTile = new idFile_Memory("mega_memory", (char *)megaMemory, len);

	int currentMegaTilePosition = 0;
	byte *megaMemoryTile = new byte[len];

	byte *compressed_tile_buffer = (byte *)R_StaticAlloc(TILE_SIZE * TILE_SIZE);

	int albedoSourcebpp = R_GetTargaBPP(albedoSource.targa_header);
	int litSourcebpp = R_GetTargaBPP(litSource.targa_header);

	int albedoSourceLen = (albedoSource.columns * albedoSourcebpp);
	albedoSourceLen = albedoSourceLen * TILE_SIZE;

	int litSourceLen = (litSource.columns * litSourcebpp);
	litSourceLen = litSourceLen * TILE_SIZE;

	albedoSource.AllocScratch(albedoSourceLen);

	int numLitBlocksToSkip = albedoSource.targa_header.width / litSource.targa_header.width;
	int litSkippedBlocks = 0;

	// Lit source will contain numLitBlocksToSkip if we are scaling!
	litSource.AllocScratch(litSourceLen);
	byte	*targa_lit = new byte[((TILE_SIZE* numLitBlocksToSkip) * albedoSource.targa_header.width) * 4];

	int blockRowsRemaining = mtHeader.tilesHigh;
	while (blockRowsRemaining--) {
		common->Printf("%i blockRowsRemaining\n", blockRowsRemaining);
		session->UpdateScreen();

		// Do a single big read here, small byte reads off disc are just slow.
		albedoSource.file->Read(albedoSource.scratch, albedoSourceLen);
		
		// Process the lit and albedo source images.
		albedoSource.ResetScratch();
		ProcessTGABlock(&albedoSource, targa_rgba, albedoSource.targa_header, albedoSource.columns, albedoSource.rows, 1);


		// Only load litSource if we need another block.
		if (numLitBlocksToSkip == 1 || litSkippedBlocks == 0)
		{
			litSource.file->Read(litSource.scratch, litSourceLen);

			litSource.ResetScratch();
			ProcessTGABlock(&litSource, targa_lit, litSource.targa_header, litSource.columns, litSource.rows, numLitBlocksToSkip);
		}

		// This is to support MegaLight not outputing lightmaps 1:1 with the size of the megatexture albedo.
		int lightmap_start = ((TILE_SIZE * albedoSource.targa_header.width)) * litSkippedBlocks; //litSourceLen * litSkippedBlocks;
		for (int i = 0; i < TILE_SIZE * albedoSource.targa_header.width; i++)
		{
			int lightmapPosition = lightmap_start + i;

			byte litR = ChannelBlend_Add(targa_lit[(lightmapPosition * 4) + 0], r_megatexture_ambient.GetInteger());
			byte litG = ChannelBlend_Add(targa_lit[(lightmapPosition * 4) + 1], r_megatexture_ambient.GetInteger());
			byte litB = ChannelBlend_Add(targa_lit[(lightmapPosition * 4) + 2], r_megatexture_ambient.GetInteger());

			targa_compose[(i * 4) + 0] = ChannelBlend_Multiply(targa_rgba[(i * 4) + 0], litR);
			targa_compose[(i * 4) + 1] = ChannelBlend_Multiply(targa_rgba[(i * 4) + 1], litG);
			targa_compose[(i * 4) + 2] = ChannelBlend_Multiply(targa_rgba[(i * 4) + 2], litB);
			targa_compose[(i * 4) + 3] = 255;
		}

		if (litSkippedBlocks >= numLitBlocksToSkip - 1) {
			litSkippedBlocks = 0;
		}
		else {
			litSkippedBlocks++;
		}

		//
		// write out individual blocks from the full row block buffer
		//
		for (int rowBlock = 0; rowBlock < mtHeader.tilesWide; rowBlock++) {
			idDxtEncoder encoder;

			//megaMemoryTile->Seek(0, FS_SEEK_SET);
			currentMegaTilePosition = 0;
			for (int y = 0; y < TILE_SIZE; y++) {
				memcpy(&megaMemoryTile[currentMegaTilePosition], targa_compose + (y * albedoSource.targa_header.width + rowBlock * TILE_SIZE) * 4, TILE_SIZE * 4);
				currentMegaTilePosition += TILE_SIZE * 4;
			}

			int fileWriteAddress = out->Tell();

			// convert the image data to YCoCg and use the YCoCgDXT5 compressor
			idColorSpace::ConvertRGBToCoCg_Y((byte *)megaMemoryTile, (byte *)megaMemoryTile, TILE_SIZE, TILE_SIZE);

			encoder.CompressYCoCgDXT5Fast_Generic((const byte *)megaMemoryTile, compressed_tile_buffer, TILE_SIZE, TILE_SIZE);

			//{
			//	idDxtDecoder decoder;
			//	byte *uncompress_test_me = (byte *)_alloca(TILE_SIZE * TILE_SIZE * 4);
			//	
			//	decoder.DecompressImageDXT5(compressed_tile_buffer, uncompress_test_me, TILE_SIZE, TILE_SIZE);
			//	R_WriteTGA("test_compression.tga", uncompress_test_me, TILE_SIZE, TILE_SIZE);
			//}

			out->Write(compressed_tile_buffer, TILE_SIZE * TILE_SIZE);
		}
	}

	delete megaMemoryTile;
	delete targa_lit;

	R_StaticFree(targa_rgba);
	R_StaticFree(targa_compose);
	R_StaticFree(compressed_tile_buffer);

	GenerateMegaMipMaps(&mtHeader, out);

	delete out;
	delete albedoSource.file;
	delete litSource.file;

	GenerateMegaPreview(outName.c_str());
#if 0
	if ((targa_header.attributes & (1 << 5))) {			// image flp bit
		R_VerticalFlip(*pic, *width, *height);
	}
#endif

}


