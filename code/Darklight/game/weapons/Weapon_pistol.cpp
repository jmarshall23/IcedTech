// Weapon_pistol.cpp
//

#include "game_precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponPistol)
END_CLASS

#define PISTOL_FIRERATE			0.4
#define PISTOL_LOWAMMO			4
#define PISTOL_NUMPROJECTILES	1

// blend times
#define PISTOL_IDLE_TO_LOWER	2
#define PISTOL_IDLE_TO_FIRE		1
#define	PISTOL_IDLE_TO_RELOAD	3
#define PISTOL_RAISE_TO_IDLE	3
#define PISTOL_FIRE_TO_IDLE		4
#define PISTOL_RELOAD_TO_IDLE	40

/*
===============
rvmWeaponPistol::Init
===============
*/
void rvmWeaponPistol::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	next_attack = 0;
	spread = weapon->GetFloat("spread");
	weapon->WeaponState(WP_RISING, 0);

	snd_lowammo = declManager->FindSound("snd_lowammo");
}

/*
===============
rvmWeaponPistol::Raise
===============
*/
void rvmWeaponPistol::Raise() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, PISTOL_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, PISTOL_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
			isRisen = true;
		}
		break;
	}
}


/*
===============
rvmWeaponPistol::Lower
===============
*/
void rvmWeaponPistol::Lower() {
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
rvmWeaponPistol::Idle
===============
*/
void rvmWeaponPistol::Idle() {
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch (idleState)
	{
	case IDLE_NOTSET:
		owner->Event_WeaponReady();
		if(!owner->AmmoInClip())
		{
			owner->Event_PlayCycle(ANIMCHANNEL_ALL, "idle_empty");
		}
		else
		{
			owner->Event_PlayCycle(ANIMCHANNEL_ALL, "idle");
		}
		idleState = IDLE_WAIT;
		break;

	case IDLE_WAIT:
		// Do nothing.
		break;
	}
}

/*
===============
rvmWeaponPistol::HasWaitSignal
===============
*/
bool rvmWeaponPistol::HasWaitSignal(void) {
	if (rvmWeaponObject::HasWaitSignal())
		return true;

	return next_attack >= gameLocal.realClientTime;
}

/*
===============
rvmWeaponPistol::Fire
===============
*/
void rvmWeaponPistol::Fire() {
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

	if(ammoClip == 0 && owner->AmmoAvailable() && firingState == 0) {
		owner->WeaponState(WP_RELOAD, PISTOL_IDLE_TO_RELOAD);
		return;
	}
	
	switch (firingState)
	{
	case FIRE_NOTSET:
		next_attack = gameLocal.realClientTime + MS2SEC(PISTOL_FIRERATE);

		if (ammoClip == PISTOL_LOWAMMO) {
			int length;
			owner->StartSoundShader(snd_lowammo, SND_CHANNEL_ITEM, 0, false, &length);
		}

		owner->Event_LaunchProjectiles(PISTOL_NUMPROJECTILES, spread, 0, 1, 1);

		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "fire", false);
		firingState = FIRE_WAIT;
		break;

	case FIRE_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, PISTOL_FIRE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, PISTOL_FIRE_TO_IDLE);
			firingState = 0;
		}
		break;
	}
}


/*
===============
rvmWeaponPistol::Reload
===============
*/
void rvmWeaponPistol::Reload() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, PISTOL_RELOAD_TO_IDLE))
		{
			owner->Event_AddToClip(owner->ClipSize());
			owner->WeaponState(WP_IDLE, PISTOL_RELOAD_TO_IDLE);
			reloadState = 0;
		}
		break;
	}
}