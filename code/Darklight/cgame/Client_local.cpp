// Client_local.cpp
//

#include "Game_precompiled.h"
#include "Client_local.h"

rvmClientGameLocal clientLocal;

/*
==================
rvmClientGameLocal::Init
==================
*/
void rvmClientGameLocal::Init(void) {
	playerView = NULL;
}

/*
==================
rvmClientGameLocal::BeginLevel
==================
*/
void rvmClientGameLocal::BeginLevel(void) {
	if (playerView != NULL) {
		common->FatalError("ClientLocal::BeginLevel: playerView != NULL\n");
	}

	// Create the playerView object.
	playerView = new idPlayerView();
	playerView->ClearEffects();
}

/*
==================
rvmClientGameLocal::MapShutdown
==================
*/
void rvmClientGameLocal::MapShutdown(void) {
	if(playerView != NULL) {
		delete playerView;
		playerView = NULL;
	}
}

/*
==================
rvmClientGameLocal::RenderPlayerView
==================
*/
void rvmClientGameLocal::RenderPlayerView(int clientNum) {
	idPlayer* player = gameLocal.entities[clientNum]->Cast<idPlayer>();
	if(player == NULL) {
		return;
	}

	playerView->SetPlayerEntity(player);
	playerView->RenderPlayerView(player->hud);
}

/*
==================
rvmClientGameLocal::InitClientGame
==================
*/
void idGameLocal::InitClientGame(void) {
	clientLocal.Init();
}

/*
==================
rvmClientGameLocal::ShutdownClientMap
==================
*/
void idGameLocal::ShutdownClientMap(void) {
	clientLocal.MapShutdown();
}

/*
================
idGameLocal::Draw

makes rendering and sound system calls
================
*/
bool idGameLocal::Draw(int clientNum) {
	clientLocal.RenderPlayerView(gameLocal.localClientNum);
	return true;
}