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
	clientSpawnCount = INITIAL_SPAWN_COUNT;

	// Create the various jobs we need on the game side.
	clientPhysicsJob = parallelJobManager->AllocJobList(JOBLIST_GAME_CLIENTPHYSICS, JOBLIST_PRIORITY_MEDIUM, 2, 0, NULL);
	parallelJobManager->RegisterJob((jobRun_t)rvmClientGameLocal::ClientEntityJob_t, "G_ClientPhysics");
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

	clientSpawnCount = 0;
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
===================
rvmClientGameLocal::SpawnClientEntityDef

Finds the spawn function for the client entity and calls it,
returning false if not found
===================
*/
bool rvmClientGameLocal::SpawnClientEntityDef(const idDict& args, rvClientEntity** cent, bool setDefaults, const char* spawn) {
	const char* classname;
	idTypeInfo* cls;
	idClass* obj;
	idStr		error;
	const char* name;

	if (cent) {
		*cent = NULL;
	}

	gameLocal.spawnArgs = args;

	if (gameLocal.spawnArgs.GetBool("nospawn")) {
		//not meant to actually spawn, just there for some compiling process
		return false;
	}

	if (gameLocal.spawnArgs.GetString("name", "", &name)) {
		error = va(" on '%s'", name);
	}

	gameLocal.spawnArgs.GetString("classname", NULL, &classname);

	const idDeclEntityDef* def = gameLocal.FindEntityDef(classname, false);
	if (!def) {
		// RAVEN BEGIN
		// jscott: a NULL classname would crash Warning()
		if (classname) {
			gameLocal.Warning("Unknown classname '%s'%s.", classname, error.c_str());
		}
		// RAVEN END
		return false;
	}

	gameLocal.spawnArgs.SetDefaults(&def->dict);

	// check if we should spawn a class object
	if (spawn == NULL) {
		gameLocal.spawnArgs.GetString("spawnclass", NULL, &spawn);
	}

	if (spawn) {

		cls = idClass::GetClass(spawn);
		if (!cls) {
			gameLocal.Warning("Could not spawn '%s'.  Class '%s' not found%s.", classname, spawn, error.c_str());
			return false;
		}

		obj = cls->CreateInstance();
		if (!obj) {
			gameLocal.Warning("Could not spawn '%s'. Instance could not be created%s.", classname, error.c_str());
			return false;
		}

		obj->CallSpawn();

		*cent = static_cast<rvClientEntity*>(obj);		

		return true;
	}

	gameLocal.Warning("%s doesn't include a spawnfunc%s.", classname, error.c_str());
	return false;
}


/*
===================
rvmClientGameLocal::RegisterClientEntity
===================
*/
void rvmClientGameLocal::RegisterClientEntity(rvClientEntity* cent) {
	int entityNumber;

	assert(cent);

	if (clientSpawnCount >= (1 << (32 - CENTITYNUM_BITS))) {
		//		Error( "idGameLocal::RegisterClientEntity: spawn count overflow" );
		clientSpawnCount = INITIAL_SPAWN_COUNT;
	}

	// Find a free entity index to use
	while (clientEntities[firstFreeClientIndex] && firstFreeClientIndex < MAX_CENTITIES) {
		firstFreeClientIndex++;
	}

	if (firstFreeClientIndex >= MAX_CENTITIES) {
		cent->PostEventMS(&EV_Remove, 0);
		gameLocal.Warning("idGameLocal::RegisterClientEntity: no free client entities");
		return;
	}

	entityNumber = firstFreeClientIndex++;

	// Add the client entity to the lists
	clientEntities[entityNumber] = cent;
	clientSpawnIds[entityNumber] = clientSpawnCount++;
	cent->entityNumber = entityNumber;
	cent->spawnNode.AddToEnd(clientSpawnedEntities);
	cent->spawnArgs.TransferKeyValues(gameLocal.spawnArgs);

	if (entityNumber >= num_clientEntities) {
		num_clientEntities++;
	}

	activeClientEntities.Append(cent);
}

/*
===================
rvmClientGameLocal::UnregisterClientEntity
===================
*/
void rvmClientGameLocal::UnregisterClientEntity(rvClientEntity* cent) {
	assert(cent);

	// No entity number then it failed to register
	if (cent->entityNumber == -1) {
		return;
	}

	cent->spawnNode.Remove();
	cent->bindNode.Remove();

	if (clientEntities[cent->entityNumber] == cent) {
		clientEntities[cent->entityNumber] = NULL;
		clientSpawnIds[cent->entityNumber] = -1;
		if (cent->entityNumber < firstFreeClientIndex) {
			firstFreeClientIndex = cent->entityNumber;
		}
		cent->entityNumber = -1;
	}

	activeClientEntities.Remove(cent);
}

/*
===================
rvmClientGameLocal::ClientEntityJob_t
===================
*/
void rvmClientGameLocal::ClientEntityJob_t(rvmClientPhysicsJobParams_t* params) {
	for (int i = params->startClientEntity; i < params->numClientEntities; i++) {
		clientLocal.clientEntityThreadWork[i]->RunThreadedPhysics(params->clientThreadId);
	}
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

/*
================
idGameLocal::SpawnClientEntities
================
*/
void idGameLocal::SpawnClientEntities(void) {
	idDict args;

	// bdube: dummy entity for client entities with physics
	args.Clear();
	args.SetInt("spawn_entnum", ENTITYNUM_CLIENT);
	// jnewquist: Use accessor for static class type 
	if (!gameLocal.SpawnEntityType(rvClientPhysics::Type, &args, true) || !gameLocal.entities[ENTITYNUM_CLIENT]) {
		Error("Problem spawning client physics entity");
	}

	args.Clear();
	args.SetInt("spawn_entnum", ENTITYNUM_CLIENT2);
	// jnewquist: Use accessor for static class type 
	if (!gameLocal.SpawnEntityType(rvClientPhysics::Type, &args, true) || !gameLocal.entities[ENTITYNUM_CLIENT2]) {
		Error("Problem spawning client physics entity");
	}
}

