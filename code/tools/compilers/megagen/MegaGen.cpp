// MegaGen.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "MegaGen.h"

/*
===============================================

MegaGen Tool

Builds the megatexture source. Why? Because doing it in Photoshop causes computers to explode :).

===============================================
*/

/*
===================
WriteTargaHeader
===================
*/
void WriteTargaHeader(idFile *megaTarga, int width, int height)
{
	byte	buffer[18];
	int		i;

	memset(buffer, 0, 18);
	buffer[2] = 2;		// uncompressed type
	buffer[12] = width & 255;
	buffer[13] = width >> 8;
	buffer[14] = height & 255;
	buffer[15] = height >> 8;
	buffer[16] = 32;	// pixel size
	
	megaTarga->Write(buffer, sizeof(buffer));
}

/*
===================
LerpPixel
===================
*/
byte LerpPixel(byte start, byte end, byte mask) {
	float t = ((float)mask) / 255.0f; 

	return (byte)((1 - t)*start + t * end);
}

/*
===================
R_GetMegaLayerImagePixel
===================
*/
byte *R_GetMegaLayerImagePixel(MegaLayerImage_t *layer, int x, int y) {
	return &layer->data[(y * layer->width * 4) + (x * 4)];
}

/*
===================
EvaluateMegaLayer
===================
*/
void EvaluateMegaLayer(int megaSize, int megaScanLine, MegaLayer *megaLayer, byte *megaScratch) {
	float maskScanlineScale = (float)megaScanLine / (float)megaSize;

	for (int i = 0; i < megaSize; i++)
	{
		int albedoX = i % megaLayer->albedoImage.width;
		int albedoY = megaScanLine % megaLayer->albedoImage.height;

		float maskColumnScale = (float)i / (float)megaSize;

		int maskX = maskColumnScale * (float)megaLayer->maskImage.width;
		int maskY = maskScanlineScale * (float)megaLayer->maskImage.height;
		
		byte *megaLayerAlbedoData = R_GetMegaLayerImagePixel(&megaLayer->albedoImage, albedoX, albedoY);
		byte *megaLayerMaskData = R_GetMegaLayerImagePixel(&megaLayer->maskImage, maskX, maskY);

		byte mask = megaLayerMaskData[0];

		// Lerp MegaPixel against the current megaScalinePixel based on the mask.
		megaScratch[(i * 4) + 0] = LerpPixel(megaScratch[(i * 4) + 0], megaLayerAlbedoData[2], mask);
		megaScratch[(i * 4) + 1] = LerpPixel(megaScratch[(i * 4) + 1], megaLayerAlbedoData[1], mask);
		megaScratch[(i * 4) + 2] = LerpPixel(megaScratch[(i * 4) + 2], megaLayerAlbedoData[0], mask);
		megaScratch[(i * 4) + 3] = LerpPixel(megaScratch[(i * 4) + 3], megaLayerAlbedoData[3], mask);
	}
}

/*
===================
BuildMegaProject
===================
*/
void BuildMegaProject(int megaSize, idFile *megaTarga, rvmMegaProject &megaProject) {
	idTempArray<byte> megaTextureScanLine(megaSize * 4);
	idTempArray<byte> scratchScanLine(megaSize * 4);

	// Write out the targa header
	WriteTargaHeader(megaTarga, megaSize, megaSize);

	// Walk through each scanline for evaluation.
	for (int i = 0; i < megaSize; i++)
	{
		//common->Printf("Writing scanline %d/%d\n", i, megaSize);

		// Iterate over all the layers.
		for (int layerId = 0; layerId < megaProject.GetNumMegaLayers(); layerId++)
		{
			// Evaluate the mega layer.
			EvaluateMegaLayer(megaSize, i, megaProject.GetMegaLayer(layerId), scratchScanLine.Ptr());
		}

		megaTarga->Write(scratchScanLine.Ptr(), megaSize * 4);
	}
}

/*
===================
RunMegaGen_f
===================
*/
void RunMegaGen_f(const idCmdArgs &args) {
	rvmMegaProject megaProject;

	common->SetRefreshOnPrint(true);

	if (args.Argc() < 3) {
		common->Warning("Usage: megagen <mega_project> <mega_size>\n");
		common->SetRefreshOnPrint(false);
		return;
	}

	idStr megaProjectName = va("megagen/%s.megagen", args.Argv(1));
	idStr megaTargaName = va("megagen/bin/%s.tga", args.Argv(1));

	idFileScoped megaTarga(fileSystem->OpenFileWrite(megaTargaName));

	int megaSize = atoi(args.Argv(2));

	idParser parser;

	// Read the entire mega project off disk.
	if (!parser.LoadFile(megaProjectName))
	{
		common->Warning("Failed to load megaproject %s\n", megaProjectName.c_str());
		common->SetRefreshOnPrint(false);
		return;
	}

	parser.SetFlags(DECL_LEXER_FLAGS);
	
	// Parse the mega project.
	if (!megaProject.ParseProject(parser))
	{
		// ParseProject should return what went wrong.
		common->SetRefreshOnPrint(false);
		return;
	}

	// Build it!!!!
	BuildMegaProject(megaSize, megaTarga, megaProject);

	common->Printf("MegaProject built successfully!");

	common->SetRefreshOnPrint(false);
}