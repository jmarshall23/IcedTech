// ClientEffects.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"
/*
====================
rvmClientEffect::~rvmClientEffect
====================
*/
rvmClientEffect::~rvmClientEffect() {
	rvClientEntity* cent;
	// Unbinding should remove the node from the list so keep using the head until
	// there are no more entities
	for (int i = 0; i < clientEntityDebris.Num(); i++) {
		clientEntityDebris[i]->Unbind();
		delete clientEntityDebris[i];
	}
	clientEntityDebris.Clear();
}

/*
====================
rvmClientEffect::AddClientModel
====================
*/
void rvmClientEffect::Spawn(void) {
	idEntity::Spawn();

	deleteFrame = -1;
}

/*
====================
rvmClientEffect::AddClientModel
====================
*/
void rvmClientEffect::AddClientModel(const idDict& clientEntityDef, idVec3 origin, idMat3 axis, idVec3 velocity, const char* shaderName) {
	rvClientModel* cent = NULL;
	// force spawnclass to rvClientMoveable
	gameLocal.SpawnClientEntityDef(clientEntityDef, (rvClientEntity**)(&cent), false, "rvClientModel");

	if (!cent) {
		return;
	}

	cent->SetOrigin(origin);
	cent->SetAxis(axis);

	cent->SetCustomShader(shaderName);
	cent->GetPhysics()->SetLinearVelocity(velocity);
	cent->PostEventMS(&CL_FadeOut, 2500, 2500);

	clientEntityDebris.Append(cent);

	deleteFrame = gameLocal.realClientTime + SEC2MS(4);

	BecomeActive(TH_THINK);
}

/*
====================
rvmClientEffect::Think
====================
*/
void rvmClientEffect::Think(void) {
	if (gameLocal.time > deleteFrame && deleteFrame != -1) {
		PostEventMS(&EV_Remove, 0);
		return;
	}

	idEntity::Think();

	for(int i = 0; i < clientEntityDebris.Num(); i++) {
		clientEntityDebris[i]->Think();
	}
}


CLASS_DECLARATION(idEntity, rvmClientEffect_debris)
END_CLASS

/*
====================
rvmClientEffect_debris::~rvmClientEffect_debris
====================
*/
rvmClientEffect_debris::~rvmClientEffect_debris() {
	
}

/*
====================
rvmClientEffect_debris::Spawn
====================
*/
void rvmClientEffect_debris::Spawn(void) {
	BaseSpawn();

	BecomeInactive(TH_THINK); // Disable think until we launch a effect.
}

/*
====================
rvmClientEffect_debris::LaunchEffect
====================
*/
void rvmClientEffect_debris::LaunchEffect(const idDeclEntityDef **debrisArray, int debrisArraySize, idVec3 origin, idMat3 axis, const char* shaderName) {
	int speed = 100 + idMath::FRandRange(0, 100);
	int count = idMath::FRandRange(6, 12);
	idVec3 unitvel;
	idVec3 forward;
	idRandom rand;

	axis.ToAngles().ToVectors(&forward);

	forward.Normalize();

	for (int i = 0; i < count; i++) {
		idDict dict = debrisArray[(int)idMath::FRandRange(0, debrisArraySize)]->dict;

		idVec3 velocity(forward[0] + rand.CRandomFloat() * 0.9, forward[1] + rand.CRandomFloat() * 0.9, fabs(forward[2]) > 0.5 ? forward[2] * (0.2 + 0.8 * rand.RandomFloat()) : rand.RandomFloat() * 0.6);
		velocity *= speed + speed * 0.5f * rand.CRandomFloat();

		AddClientModel(dict, origin + (forward * idMath::FRandRange(1, 5)), axis, velocity, shaderName);
	}

	BecomeActive(TH_THINK);
}

/*
====================
rvmClientEffect_debris::Think
====================
*/
void rvmClientEffect_debris::Think(void) {
	rvmClientEffect::Think();
}