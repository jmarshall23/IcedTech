// Game_parallel.cpp
//

#include "precompiled.h"
#include "Game_local.h"

/*
===================
idGameLocal::InitJobSystem
===================
*/
void idGameLocal::InitJobSystem(void) {
	gameParallelJobList = parallelJobManager->AllocJobList(JOBLIST_GAME, JOBLIST_PRIORITY_MEDIUM, 4, 4, NULL);
}

/*
===================
idGameLocal::ShutdownJobSystem
===================
*/
void idGameLocal::ShutdownJobSystem(void) {
	parallelJobManager->FreeJobList(gameParallelJobList);
	gameParallelJobList = nullptr;
}