// main.cpp
//

#include "renderlight.h"
#include <sdl.h>

SDL_Window *window;

/*
=============
PrintBSPInfo

Dumps info about current file
=============
*/
void PrintBSPInfo(void) {
	LogPrint("BSP Info:\n");
	LogPrint("%6i models       %7i\n" , nummodels, (int)(nummodels * sizeof(dmodel_t)));
	LogPrint("%6i shaders      %7i\n" , numShaders, (int)(numShaders * sizeof(dshader_t)));
	LogPrint("%6i brushes      %7i\n" , numbrushes, (int)(numbrushes * sizeof(dbrush_t)));
	LogPrint("%6i brushsides   %7i\n" , numbrushsides, (int)(numbrushsides * sizeof(dbrushside_t)));
	LogPrint("%6i fogs         %7i\n" , numFogs, (int)(numFogs * sizeof(dfog_t)));
	LogPrint("%6i planes       %7i\n" , numplanes, (int)(numplanes * sizeof(dplane_t)));
	LogPrint("%6i entdata      %7i\n", num_entities, entdatasize);
	LogPrint("%6i nodes        %7i\n" , numnodes, (int)(numnodes * sizeof(dnode_t)));
	LogPrint("%6i leafs        %7i\n" , numleafs, (int)(numleafs * sizeof(dleaf_t)));
	LogPrint("%6i leafsurfaces %7i\n" , numleafsurfaces, (int)(numleafsurfaces * sizeof(dleafsurfaces[0])));
	LogPrint("%6i leafbrushes  %7i\n" , numleafbrushes, (int)(numleafbrushes * sizeof(dleafbrushes[0])));
	LogPrint("%6i drawverts    %7i\n" , numDrawVerts, (int)(numDrawVerts * sizeof(drawVerts[0])));
	LogPrint("%6i drawindexes  %7i\n" , numDrawIndexes, (int)(numDrawIndexes * sizeof(drawIndexes[0])));
	LogPrint("%6i drawsurfaces %7i\n" , numDrawSurfaces, (int)(numDrawSurfaces * sizeof(drawSurfaces[0])));
	LogPrint("%6i lightmaps    %7i\n" , numLightBytes / (LIGHTMAP_WIDTH*LIGHTMAP_HEIGHT * 3), numLightBytes);
	LogPrint("       visibility   %7i\n" , numVisBytes);
}

int main(int argc, char **argv)
{
	LogPrint("RenderLight v0.01 Build(%s,%s) by Justin Marshall for idTech 3\n", __DATE__, __TIME__);

	if (argc < 2)
	{
		LogPrint("Invalid usage!\n");
		return -1;
	}

	// Load in our bsp file.
	LogPrint("Loading BSP file: %s\n", argv[1]);
	LoadBSPFile(argv[1]);

	// Create tool window.
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		argv[1],                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		1024,                               // width, in pixels
		768,                               // height, in pixels
		SDL_WINDOW_BORDERLESS                  // flags - see below
	);


	// Dump the BSP info to the screen(basically to ensure we loaded everything properly).
	PrintBSPInfo();

	return 0;
}

