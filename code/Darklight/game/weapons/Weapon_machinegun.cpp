// Weapon_machinegun.cpp
//

#include "game_precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponMachineGun)
END_CLASS

#define MACHINEGUN_FIRERATE			0.1
#define MACHINEGUN_LOWAMMO			10
#define MACHINEGUN_NUMPROJECTILES	1

// blend times
#define MACHINEGUN_IDLE_TO_LOWER	4
#define MACHINEGUN_IDLE_TO_FIRE		0
#define	MACHINEGUN_IDLE_TO_RELOAD	4
#define MACHINEGUN_RAISE_TO_IDLE	4
#define MACHINEGUN_FIRE_TO_IDLE		0
#define MACHINEGUN_RELOAD_TO_IDLE	4


/*
===============
rvmWeaponMachineGun::Init
===============
*/
void rvmWeaponMachineGun::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	next_attack = 0;
	spread = weapon->GetFloat("spread");
	weapon->WeaponState(WP_RISING, 0);

	snd_lowammo = declManager->FindSound("snd_lowammo");
}

/*
===============
rvmWeaponMachineGun::Raise
===============
*/
void rvmWeaponMachineGun::Raise() {
	enum RisingState
	{
		RISING_NOTSET = 0,
		RISING_WAIT
	};

	switch (risingState)
	{
	case RISING_NOTSET:
		owner->Event_WeaponRising();
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "raise", false);
		risingState = RISING_WAIT;
		break;

	case RISING_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, MACHINEGUN_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, MACHINEGUN_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
			isRisen = true;
		}
		break;
	}
}


/*
===============
rvmWeaponMachineGun::Lower
===============
*/
void rvmWeaponMachineGun::Lower() {
	enum LoweringState
	{
		LOWERING_NOTSET = 0,
		LOWERING_WAIT
	};

	switch (loweringState)
	{
	case LOWERING_NOTSET:
		owner->Event_WeaponLowering();
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "putaway", false);
		loweringState = LOWERING_WAIT;
		break;

	case LOWERING_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, 0))
		{
			owner->Event_WeaponHolstered();
			loweringState = LOWERING_NOTSET;
			isHolstered = true;
		}
		break;
	}
}

/*
===============
rvmWeaponMachineGun::Idle
===============
*/
void rvmWeaponMachineGun::Idle() {
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch (idleState)
	{
	case IDLE_NOTSET:
		owner->Event_WeaponReady();
		owner->Event_PlayCycle(ANIMCHANNEL_ALL, "idle");
		idleState = IDLE_WAIT;
		break;

	case IDLE_WAIT:
		// Do nothing.
		break;
	}
}

/*
===============
rvmWeaponMachineGun::HasWaitSignal
===============
*/
bool rvmWeaponMachineGun::HasWaitSignal(void) {
	if (rvmWeaponObject::HasWaitSignal())
		return true;

	return next_attack >= gameLocal.realClientTime;
}

/*
===============
rvmWeaponMachineGun::Fire
===============
*/
void rvmWeaponMachineGun::Fire() {
	int ammoClip = owner->AmmoInClip();

	if (next_attack >= gameLocal.realClientTime)
	{
		return;
	}

	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_WAIT
	};

	if (ammoClip == 0 && owner->AmmoAvailable() && firingState == 0) {
		owner->WeaponState(WP_RELOAD, MACHINEGUN_IDLE_TO_RELOAD);
		return;
	}

	switch (firingState)
	{
	case FIRE_NOTSET:
		next_attack = gameLocal.realClientTime + SEC2MS(MACHINEGUN_FIRERATE);
		owner->Event_LaunchProjectiles(MACHINEGUN_NUMPROJECTILES, spread, 0, 1, 1);

		// Create debris if we hit a wall.
		//{
		//	trace_t tr;
		//	idVec3 end;
		//
		//	end = owner->GetOrigin() + (owner->GetForwardVector() * 1000);
		//
		//	gameLocal.Trace(tr, owner->GetOrigin(), end, CONTENTS_BODY, 0);
		//	if(tr.c.entityNum == ENTITYNUM_WORLD) {
		//		const char* shaderName = tr.c.material->GetName();
		//
		//		idVec3 dir = tr.endpos - owner->GetOrigin();
		//		dir.Normalize();
		//
		//		idVec3 reflectedDir = idMath::ReflectVector(dir, tr.c.normal);
		//		gameLocal.SpawnDebris(tr.endpos, reflectedDir.ToMat3(), shaderName);
		//	}
		//}

		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "fire", false);
		owner->Event_FireSound();

		firingState = FIRE_WAIT;
		break;

	case FIRE_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, MACHINEGUN_FIRE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, MACHINEGUN_FIRE_TO_IDLE);
			firingState = 0;
		}
		break;
	}
}

/*
===============
rvmWeaponMachineGun::Reload
===============
*/
void rvmWeaponMachineGun::Reload() {
	enum RELOAD_State
	{
		RELOAD_NOTSET = 0,
		RELOAD_WAIT
	};

	switch (reloadState)
	{
	case RELOAD_NOTSET:
		owner->Event_WeaponReloading();
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "reload", false);
		reloadState = RELOAD_WAIT;
		break;

	case RELOAD_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, MACHINEGUN_RELOAD_TO_IDLE))
		{
			owner->Event_AddToClip(owner->ClipSize());
			owner->WeaponState(WP_IDLE, MACHINEGUN_RELOAD_TO_IDLE);
			reloadState = 0;
		}
		break;
	}
}