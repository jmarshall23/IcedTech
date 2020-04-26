// Client_local.cpp
//

#include "Game_precompiled.h"
#include "Client_local.h"

rvmClientLocal clientLocal;

/*
==================
rvmClientLocal::Init
==================
*/
void rvmClientLocal::Init(void) {
	playerView = NULL;
}

/*
==================
rvmClientLocal::BeginLevel
==================
*/
void rvmClientLocal::BeginLevel(void) {
	if (playerView != NULL) {
		common->FatalError("ClientLocal::BeginLevel: playerView != NULL\n");
	}

	// Create the playerView object.
	playerView = new idPlayerView();
	playerView->ClearEffects();
}

/*
==================
rvmClientLocal::MapShutdown
==================
*/
void rvmClientLocal::MapShutdown(void) {
	if(playerView != NULL) {
		delete playerView;
		playerView = NULL;
	}
}

/*
==================
rvmClientLocal::RenderPlayerView
==================
*/
void rvmClientLocal::RenderPlayerView(int clientNum) {
	idPlayer* player = gameLocal.entities[clientNum]->Cast<idPlayer>();
	if(player == NULL) {
		return;
	}

	playerView->SetPlayerEntity(player);
	playerView->RenderPlayerView(player->hud);
}

/*
==================
rvmClientLocal::InitClientGame
==================
*/
void idGameLocal::InitClientGame(void) {
	clientLocal.Init();
}

/*
==================
rvmClientLocal::ShutdownClientMap
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