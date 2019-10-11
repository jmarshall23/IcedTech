// Weapon_rocketlauncher.cpp
//


#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponRocketLauncher)
END_CLASS

#define ROCKETLAUNCHER_LOWAMMO			1
#define ROCKETLAUNCHER_NUMPROJECTILES	1
#define ROCKETLAUNCHER_FIREDELAY		1		// was .75 changed again by Tim to 1

// blend times
#define ROCKETLAUNCHER_IDLE_TO_LOWER	4
#define ROCKETLAUNCHER_IDLE_TO_FIRE		0
#define	ROCKETLAUNCHER_IDLE_TO_RELOAD	4
#define ROCKETLAUNCHER_RAISE_TO_IDLE	4
#define ROCKETLAUNCHER_FIRE_TO_IDLE		0
#define ROCKETLAUNCHER_RELOAD_TO_IDLE	4
#define ROCKETLAUNCHER_RELOAD_FRAME		34		// how many frames from the end of "reload" to fill the clip

/*
===============
rvmWeaponRocketLauncher::Init
===============
*/
void rvmWeaponRocketLauncher::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	next_attack = 0;
	spread = weapon->GetFloat("spread");
	skin_invisible = weapon->GetKey("skin_invisible");
	weapon->WeaponState(WP_RISING, 0);
}

/*
===============
rvmWeaponRocketLauncher::UpdateSkin
===============
*/
void rvmWeaponRocketLauncher::UpdateSkin() {
	idStr skinname;
	int ammoClip;

	if (owner->Event_IsInvisible()) {
		skinname = skin_invisible;
	}
	else {
		ammoClip = owner->AmmoInClip();
		if (ammoClip > 5) {
			// can happen in MP - weapon Raising with slightly out-of-sync ammoClip
			ammoClip = 5;
		}
		if (ammoClip < 0) {
			ammoClip = 0;
		}
		skinname = va("skins/models/weapons/%drox.skin", ammoClip);
	}
	owner->Event_SetSkin(skinname);
}

/*
===============
rvmWeaponRocketLauncher::Raise
===============
*/
void rvmWeaponRocketLauncher::Raise() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, ROCKETLAUNCHER_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, ROCKETLAUNCHER_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
		}
		break;
	}
}


/*
===============
rvmWeaponRocketLauncher::Lower
===============
*/
void rvmWeaponRocketLauncher::Lower() {
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
		}
		break;
	}
}

/*
===============
rvmWeaponRocketLauncher::Idle
===============
*/
void rvmWeaponRocketLauncher::Idle() {
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
rvmWeaponRocketLauncher::Fire
===============
*/
void rvmWeaponRocketLauncher::Fire() {
	int ammoClip = owner->AmmoInClip();

	if (next_attack >= MS2SEC(gameLocal.realClientTime))
	{
		return;
	}

	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_WAIT
	};

	if (ammoClip == 0 && owner->AmmoAvailable() && firingState == 0) {
		owner->WeaponState(WP_RELOAD, ROCKETLAUNCHER_IDLE_TO_RELOAD);
		return;
	}

	switch (firingState)
	{
	case FIRE_NOTSET:
		next_attack = MS2SEC(gameLocal.realClientTime) + ROCKETLAUNCHER_FIREDELAY;
		owner->Event_LaunchProjectiles(ROCKETLAUNCHER_NUMPROJECTILES, spread, 0, 1, 1);

		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "fire", false);
		firingState = FIRE_WAIT;
		break;

	case FIRE_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, ROCKETLAUNCHER_FIRE_TO_IDLE))
		{
			UpdateSkin();
			owner->WeaponState(WP_IDLE, ROCKETLAUNCHER_FIRE_TO_IDLE);
			firingState = 0;
		}
		break;
	}
}

/*
===============
rvmWeaponRocketLauncher::Reload
===============
*/
void rvmWeaponRocketLauncher::Reload() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, ROCKETLAUNCHER_RELOAD_FRAME))
		{
			owner->Event_AddToClip(owner->ClipSize());
			UpdateSkin();
			owner->WeaponState(WP_IDLE, ROCKETLAUNCHER_RELOAD_TO_IDLE);
			reloadState = 0;
		}
		break;
	}
}