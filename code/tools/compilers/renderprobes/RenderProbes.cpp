// RenderProbes.cpp
//

#include "tools_precompiled.h"
#include "../../framework/Session_local.h"

extern idCVar r_useLightCulling;

/*
===================
RenderProbes_f
===================
*/
void RenderProbes_f(const idCmdArgs& args) {
	idStr map, string;
	findFile_t	ff;
	idCmdArgs	rl_args;
	int screenWidth, screenHeight;

	map = args.Argv(1);
	if (!map.Length()) {
		return;
	}
	map.StripFileExtension();

	// Disconnect from the current game.
	cmdSystem->BufferCommandText(CMD_EXEC_NOW, "disconnect");
	
	// make sure the level exists before trying to change, so that
	// a typo at the server console won't end the game
	// handle addon packs through reloadEngine
	sprintf(string, "maps/%s.map", map.c_str());
	ff = fileSystem->FindFile(string, true);
	switch (ff) {
	case FIND_NO:
		common->Printf("Can't find map %s\n", string.c_str());
		return;
	case FIND_ADDON:
		common->Printf("map %s is in an addon pak\n", string.c_str());
		return;
	default:
		break;
	}

	screenWidth = renderSystem->GetScreenWidth();
	screenHeight = renderSystem->GetScreenHeight();

	// 
	idRenderWorld* renderWorld = renderSystem->AllocRenderWorld();
	if(!renderWorld->InitFromMap(string.c_str())) {
		common->FatalError("Failed to load renderworld!\n");
		return;
	}

	int useLightCulling = r_useLightCulling.GetInteger();
	r_useLightCulling.SetInteger(0); // Disable light culling when building reflection probes.

	// Load in the new map data.
	game->InitFromNewMap(string.c_str(), renderWorld, sessLocal.menuSoundWorld, true, false, 1, true);

	// Run a couple frames to settle everything down.
	for (int i = 0; i < 3; i++)
		game->RunFrame(NULL);

	// let the renderSystem generate interactions now that everything is spawned
	renderWorld->GenerateAllInteractions();

	// Build our reflection captures.
	gameEdit->BuildReflectionCaptures();

	// Disconnect again from the map, were done building.
	game->MapShutdown();

	renderSystem->FreeRenderWorld(renderWorld);

	r_useLightCulling.SetInteger(useLightCulling);

	// Ensure the render targets are back to the correct resolution.
	renderSystem->BeginFrame(screenWidth, screenHeight);
	renderSystem->EndFrame(NULL, NULL);
	game->ResetGameRenderTargets();
}