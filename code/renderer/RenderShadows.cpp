// RenderShadows.cpp
//

#include "engine_precompiled.h"
#include "tr_local.h"

rvmRenderShadowSystem renderShadowSystem;

idCVar rvmRenderShadowSystem::r_shadowMapAtlasSize("r_shadowMapAtlasSize", "8192", CVAR_RENDERER | CVAR_INTEGER | CVAR_ROM, "size of the shadowmap atlas");
idCVar rvmRenderShadowSystem::r_shadowMapAtlasSliceSize("r_shadowMapAtlasSliceSize", "512", CVAR_RENDERER | CVAR_INTEGER | CVAR_ROM, "size of the shadow map atlas slice size");

/*
======================
rvmRenderShadowSystem::rvmRenderShadowSystem
======================
*/
rvmRenderShadowSystem::rvmRenderShadowSystem() {
	shadowMapAtlasRT = NULL;
	shadowAtlasEntries = NULL;
}

/*
======================
rvmRenderShadowSystem::Init
======================
*/
void rvmRenderShadowSystem::Init(void) {
	common->Printf("...Init Render Shadow System...\n");

	// Create the shadow map atlas image.
	{
		idImageOpts opts;
		opts.format = FMT_DEPTH;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = r_shadowMapAtlasSize.GetInteger();
		opts.height = r_shadowMapAtlasSize.GetInteger();
		opts.numMSAASamples = 0;

		idImage* depthImage = renderSystem->CreateImage("_shadowMapAtlas", &opts, TF_LINEAR);

		shadowMapAtlasRT = new idRenderTexture(NULL, depthImage);
		shadowMapAtlasRT->InitRenderTexture();
	}

	// Allocate our shadow map atlas slice entry table.
	int numEntriesPerAxis = r_shadowMapAtlasSize.GetInteger() / r_shadowMapAtlasSliceSize.GetInteger();
	shadowAtlasEntries = new rvmRenderShadowAtlasEntry[numEntriesPerAxis * numEntriesPerAxis];

	// Check to make sure the cvars are setup correctly.
	if(numEntriesPerAxis <= 0) {
		common->FatalError("Your shadow map cvars are set incorrectly r_shadowMapAtlasSize: %d r_shadowMapAtlasSliceSize: %d\n", r_shadowMapAtlasSize.GetInteger(), r_shadowMapAtlasSliceSize.GetInteger());
	}

	common->Printf("...Shadow Map Slice Table %dx%d\n", numEntriesPerAxis, numEntriesPerAxis);

	for(int y = 0; y < numEntriesPerAxis; y++) {
		for(int x = 0; x < numEntriesPerAxis; x++) {
			rvmRenderShadowAtlasEntry* entry = &shadowAtlasEntries[(y * numEntriesPerAxis) + x];

			entry->x = x * r_shadowMapAtlasSliceSize.GetInteger();
			entry->y = y * r_shadowMapAtlasSliceSize.GetInteger();

			entry->sliceSizeX = r_shadowMapAtlasSliceSize.GetInteger();
			entry->sliceSizeY = r_shadowMapAtlasSliceSize.GetInteger();

			entry->textureX = (float)x / (float)r_shadowMapAtlasSize.GetInteger();
			entry->textureY = (float)y / (float)r_shadowMapAtlasSize.GetInteger();
		}
	}

	// Create the atlas lookup texture.
	{
		idImageOpts opts;
		opts.format = FMT_RG32;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = numEntriesPerAxis * numEntriesPerAxis;
		opts.height = 1;
		opts.numMSAASamples = 0;

		atlasEntriesLookupTexture = renderSystem->CreateImage("_shadowMapAtlasLookup", &opts, TF_NEAREST);
		
		idTempArray<idVec2>	rawData(opts.width);

		for(int i = 0; i < opts.width; i++) {
			rawData[i].x = shadowAtlasEntries[i].textureX;
			rawData[i].y = shadowAtlasEntries[i].textureY;
		}

		atlasEntriesLookupTexture->SubImageUpload(0, 0, 0, 0, opts.width, 1, (const void*)rawData.Ptr());
	}
}

/*
======================
rvmRenderShadowSystem::Shutdown
======================
*/
void rvmRenderShadowSystem::Shutdown(void) {
	if(shadowMapAtlasRT != NULL) {
		delete shadowMapAtlasRT;
		shadowMapAtlasRT = NULL;
	}

	if(shadowAtlasEntries != NULL) {
		delete shadowAtlasEntries;
		shadowAtlasEntries = NULL;
	}
}