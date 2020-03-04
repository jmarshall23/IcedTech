// Weapon_plasmagun.cpp
//

#include "Game_precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponPlasmaGun)
END_CLASS

#define PLASMAGUN_FIRERATE			0.125 //changed by Tim
#define PLASMAGUN_LOWAMMO			10
#define PLASMAGUN_NUMPROJECTILES	1

// blend times
#define PLASMAGUN_IDLE_TO_LOWER		4
#define PLASMAGUN_IDLE_TO_FIRE		1
#define	PLASMAGUN_IDLE_TO_RELOAD	4
#define PLASMAGUN_RAISE_TO_IDLE		4
#define PLASMAGUN_FIRE_TO_IDLE		4
#define PLASMAGUN_RELOAD_TO_IDLE	4

/*
===============
rvmWeaponPlasmaGun::Init
===============
*/
void rvmWeaponPlasmaGun::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	next_attack = 0;
	spread = weapon->GetFloat("spread");
	weapon->WeaponState(WP_RISING, 0);

	snd_lowammo = declManager->FindSound("snd_lowammo");
}

/*
===============
rvmWeaponPlasmaGun::Raise
===============
*/
void rvmWeaponPlasmaGun::Raise() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, PLASMAGUN_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, PLASMAGUN_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
			isRisen = true;
		}
		break;
	}
}


/*
===============
rvmWeaponPlasmaGun::Lower
===============
*/
void rvmWeaponPlasmaGun::Lower() {
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
rvmWeaponPlasmaGun::Idle
===============
*/
void rvmWeaponPlasmaGun::Idle() {
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
rvmWeaponPlasmaGun::HasWaitSignal
===============
*/
bool rvmWeaponPlasmaGun::HasWaitSignal(void) {
	if (rvmWeaponObject::HasWaitSignal())
		return true;

	return next_attack >= gameLocal.time;
}

/*
===============
rvmWeaponPlasmaGun::Fire
===============
*/
void rvmWeaponPlasmaGun::Fire() {
	int ammoClip = owner->AmmoInClip();

	if (next_attack >= gameLocal.time)
	{
		return;
	}

	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_WAIT
	};

	if (ammoClip == 0 && owner->AmmoAvailable() && firingState == 0) {
		owner->WeaponState(WP_RELOAD, PLASMAGUN_IDLE_TO_RELOAD);
		return;
	}

	switch (firingState)
	{
	case FIRE_NOTSET:
		next_attack = gameLocal.time + MS2SEC(PLASMAGUN_FIRERATE);
		owner->Event_LaunchProjectiles(PLASMAGUN_NUMPROJECTILES, spread, 0, 1, 1);

		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "fire", false);
		firingState = FIRE_WAIT;
		break;

	case FIRE_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, PLASMAGUN_FIRE_TO_IDLE))
		{
			if(!owner->IsFiring() && owner->AmmoInClip() && gameLocal.time >= next_attack)
			{
				owner->WeaponState(WP_IDLE, PLASMAGUN_FIRE_TO_IDLE);
			}
			firingState = 0;
		}
		break;
	}
}

/*
===============
rvmWeaponPlasmaGun::Reload
===============
*/
void rvmWeaponPlasmaGun::Reload() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, PLASMAGUN_RELOAD_TO_IDLE))
		{
			owner->Event_AddToClip(owner->ClipSize());
			owner->WeaponState(WP_IDLE, PLASMAGUN_RELOAD_TO_IDLE);
			reloadState = 0;
		}
		break;
	}
}