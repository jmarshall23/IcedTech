// Model_Terrain.cpp
//

#include "Engine_precompiled.h"

#include "tr_local.h"
#include "Model_local.h"

/*
============================
rvmRenderModelTerrain::InitFromFile
============================
*/
void rvmRenderModelTerrain::InitFromFile(const char *fileName) {
	idStr megaTexturePath;
	idStr fileNameOnly = fileName;

	fileNameOnly.StripPath();
	fileNameOnly.StripFileExtension();

	// Parse the terrain file.
	if (!ParseFile(fileName))
		return;

	// Load in the megatexture.
	megaTexturePath = va("megatextures/%s", fileNameOnly.c_str());
	megaTextureMaterial = declManager->FindMaterial(megaTexturePath.c_str());

	// Build the terrain from the heightmap
	BuildTerrain();

	// it is now available for use
	purged = false;

	// create the bounds for culling and dynamic surface creation
	FinishSurfaces();
}

/*
============================
rvmRenderModelTerrain::BuildTerrain
============================
*/
void rvmRenderModelTerrain::BuildTerrain(void) {
	if (NumSurfaces() != 1)
	{
		common->FatalError("rvmRenderModelTerrain::BuildTerrain: Bad surface count on terrain!");
		return;
	}

	modelSurface_t *surface = &surfaces[0];

	for (int i = 0; i < surface->geometry->numVerts; i++)
	{
		surface->shader = megaTextureMaterial;

		idVec2 st = surface->geometry->verts[i].st;
		st.x *= heightMap.width;
		st.y *= heightMap.height;

		if (st.y > 0)
			st.y = st.y - 1.0f;

		surface->geometry->verts[i].xyz.z = GetHeightForPixel(st.x, st.y) * 5000.0f;
	}
}

/*
============================
rvmRenderModelTerrain::GetHeightForPixel
============================
*/
float rvmRenderModelTerrain::GetHeightForPixel(int x, int y) {
	return heightMap.buffer[(x + (y * heightMap.width))].r / 255.0f;
}

/*
============================
rvmRenderModelTerrain::ParseFile
============================
*/
bool rvmRenderModelTerrain::ParseFile(const char *fileName) {
	idLexer parser;
	idToken token;

	if (!parser.LoadFile(fileName))
	{
		common->Warning("rvmRenderModelTerrain::ParseFile: Failed to load file %s\n", fileName);
		return false;
	}

	parser.SetFlags(DECL_LEXER_FLAGS);

	while (parser.ReadToken(&token))
	{
		if (token == "heightmap")
		{
			parser.ReadToken(&token);
			R_LoadTGA(token.c_str(), (byte **)&heightMap.buffer, &heightMap.width, &heightMap.height, nullptr);
		}
		else if (token == "basemesh")
		{
			parser.ReadToken(&token);
			LoadOBJ(token);
		}
		else
		{
			common->FatalError("Unknown or unexpected token while parsing terrain: %s\n", token.c_str());
		}
	}

	// Check to see if we have loaded the heightmap.
	if (heightMap.buffer == nullptr || heightMap.width <= 0 || heightMap.height <= 0)
	{
		common->FatalError("Terrain is invalid, heightmap isn't setup correctly.");
	}

	return true;
}