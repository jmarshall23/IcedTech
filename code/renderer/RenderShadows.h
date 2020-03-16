// RenderShadows.h
//

//
// rvmRenderShadowAtlasEntry
//
struct rvmRenderShadowAtlasEntry {
	rvmRenderShadowAtlasEntry() {
		x = -1;
		y = -1;
		sliceSizeX = -1;
		sliceSizeY = -1;
		textureX = -1;
		textureY = -1;
	}

	int x;
	int y;
	int sliceSizeX;
	int sliceSizeY;
	float textureX;
	float textureY;
};

//
// rvmRenderShadowSystem
//
class rvmRenderShadowSystem {
public:
	rvmRenderShadowSystem();

	// Inits the render shadow system.
	void				Init(void);

	// Shutdowns the render shadow system.
	void				Shutdown(void);

	// Returns the shadow map depth atlas image.
	idImage*			GetShadowMapDepthAtlas(void) { return shadowMapAtlasRT->GetDepthImage(); }

	// Returns the shadow map depth atlas render target.
	idRenderTexture*	GetShadowMapDepthAtlasRT(void) { return shadowMapAtlasRT; }

	// Returns the size of the shadow map;
	int					GetShadowMapSize(void) { return GetShadowMapDepthAtlas()->GetOpts().width; }

	// Returns the atlas lookup texture.
	idImage*			GetAtlasLookupImage(void) { return atlasEntriesLookupTexture; }

	// Returns the sample scale.
	float				GetAtlasSampleScale(void) { return (float)r_shadowMapAtlasSliceSize.GetInteger() / (float)r_shadowMapAtlasSize.GetInteger(); }

	// Returns the size of the shadow map atlas.
	float				GetShadowMapAtlasSize(void) { return (float)r_shadowMapAtlasSize.GetInteger(); }

	// Returns the atlas entry for the requested index.
	rvmRenderShadowAtlasEntry* GetShadowAtlasEntry(int idx) { return &shadowAtlasEntries[idx]; }
private:
	static idCVar		r_shadowMapAtlasSize;
	static idCVar		r_shadowMapAtlasSliceSize;
private:
	idImage*			atlasEntriesLookupTexture;

	idRenderTexture*	shadowMapAtlasRT;
	rvmRenderShadowAtlasEntry* shadowAtlasEntries;
};

extern rvmRenderShadowSystem renderShadowSystem;