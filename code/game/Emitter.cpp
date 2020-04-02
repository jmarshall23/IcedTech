// Emitter.cpp
//

#include "Game_precompiled.h"

/*
=====================================

rvEmitter

Server side entity for FX. If you want to create non level effects, just call gameLocal.CreateEffect directly,
we don't want to sync all the FX bits.

=====================================
*/

CLASS_DECLARATION(idEntity, rvmEmitter)
END_CLASS

/*
=====================
rvmEmitter::Spawn
=====================
*/
void rvmEmitter::Spawn(void) {
	BaseSpawn();

	idStr fxName = spawnArgs.GetString("fx", "");
	if(fxName.Length() <= 0) {
		common->Warning("rvmEmitter with no FX!\n");
		return;
	}

	gameLocal.CreateEffect(fxName, GetPhysics()->GetOrigin(), GetPhysics()->GetAxis(), true);
}