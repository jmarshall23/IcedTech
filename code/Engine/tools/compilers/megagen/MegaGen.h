// MegaGen.h
//

#pragma once

//
// MegaLayerImage_t
//
struct MegaLayerImage_t {
	MegaLayerImage_t()
	{
		width = -1;
		height = -1;
		data = nullptr;
	}
	
	~MegaLayerImage_t()
	{
		if (data != nullptr)
		{
			Mem_Free(data);
			data = nullptr;
		}
	}

	int width;
	int height;
	byte *data;
};

//
// MegaLayer
//
struct MegaLayer {
	MegaLayerImage_t albedoImage;
	MegaLayerImage_t maskImage;
};

//
// rvmMegaProject
//
class rvmMegaProject {
public:
	rvmMegaProject();
	~rvmMegaProject();

	// Parsers the project.
	bool ParseProject(idParser &parser);

	// Returns the megalayer given idx.
	MegaLayer *GetMegaLayer(int idx) { return megaLayers[idx]; }

	// Returns the number of mega layers in the project.
	int GetNumMegaLayers(void) const { return megaLayers.Num(); }
private:
	// Parses each layer.
	bool ParseLayer(MegaLayer *layer, idStr &layerStr);

private:
	idList<MegaLayer *>	megaLayers;
};

void R_LoadTGA(const char *name, byte **pic, int *width, int *height, ID_TIME_T *timestamp);
