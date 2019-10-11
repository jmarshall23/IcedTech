// Weapon_shotgun.cpp
//

#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponShotgun)
END_CLASS

#define SHOTGUN_FIRERATE		1.333
#define SHOTGUN_LOWAMMO			2
#define SHOTGUN_RELOADRATE		2
#define	SHOTGUN_NUMPROJECTILES	13

// blend times
#define SHOTGUN_IDLE_TO_IDLE	0
#define SHOTGUN_IDLE_TO_LOWER	4
#define SHOTGUN_IDLE_TO_FIRE	1
#define	SHOTGUN_IDLE_TO_RELOAD	4
#define	SHOTGUN_IDLE_TO_NOAMMO	4
#define SHOTGUN_NOAMMO_TO_RELOAD 4
#define SHOTGUN_NOAMMO_TO_IDLE	4
#define SHOTGUN_RAISE_TO_IDLE	1
#define SHOTGUN_FIRE_TO_IDLE	4
#define SHOTGUN_RELOAD_TO_IDLE	4
#define	SHOTGUN_RELOAD_TO_FIRE	4
#define SHOTGUN_RELOAD_TO_LOWER 2

/*
===============
rvmWeaponShotgun::Init
===============
*/
void rvmWeaponShotgun::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	next_attack = 0;
	spread = weapon->GetFloat("spread"); // weapon->GetFloat("spread")
	weapon->WeaponState(WP_RISING, 0);

	snd_lowammo = declManager->FindSound("snd_lowammo");
}

/*
===============
rvmWeaponShotgun::Raise
===============
*/
void rvmWeaponShotgun::Raise() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, SHOTGUN_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, SHOTGUN_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
		}
		break;
	}
}


/*
===============
rvmWeaponShotgun::Lower
===============
*/
void rvmWeaponShotgun::Lower() {
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
rvmWeaponShotgun::Idle
===============
*/
void rvmWeaponShotgun::Idle() {
	float currentTime;
	float clip_size;

	clip_size = owner->ClipSize();

	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch (idleState)
	{
	case IDLE_NOTSET:
		owner->Event_WeaponReady();
		if (!owner->AmmoInClip())
		{
			owner->Event_WeaponOutOfAmmo();
		}
		else
		{
			owner->Event_WeaponReady();
		}

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
rvmWeaponShotgun::Fire
===============
*/
void rvmWeaponShotgun::Fire() {
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
		owner->WeaponState(WP_RELOAD, SHOTGUN_IDLE_TO_RELOAD);
		return;
	}

	switch (firingState)
	{
	case FIRE_NOTSET:
		next_attack = MS2SEC(gameLocal.realClientTime) + SHOTGUN_FIRERATE;

		if (ammoClip == SHOTGUN_LOWAMMO) {
			int length;
			owner->StartSoundShader(snd_lowammo, SND_CHANNEL_ITEM, 0, false, &length);
		}

		owner->Event_LaunchProjectiles(SHOTGUN_NUMPROJECTILES, spread, 0, 1, 1);

		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "fire", false);
		firingState = FIRE_WAIT;
		break;

	case FIRE_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, SHOTGUN_FIRE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, SHOTGUN_FIRE_TO_IDLE);
			firingState = 0;
		}
		break;
	}
}

/*
===============
rvmWeaponShotgun::HasWaitSignal
===============
*/
bool rvmWeaponShotgun::HasWaitSignal(void) {
	if (rvmWeaponObject::HasWaitSignal())
		return true;

	// Allow the player to shoot or reload while reloading.
	return false; // next_attack >= MS2SEC(gameLocal.realClientTime);
}


/*
===============
rvmWeaponShotgun::Reload
===============
*/
void rvmWeaponShotgun::Reload() {
	float ammoClip;
	float ammoAvail;
	float clip_size;

	clip_size = owner->ClipSize();

	enum RELOAD_State
	{
		RELOAD_NOTSET = 0,
		RELOAD_WAIT
	};

	ammoAvail = owner->AmmoAvailable();
	ammoClip = owner->AmmoInClip();

	switch (reloadState)
	{
	case RELOAD_NOTSET:
		owner->Event_WeaponReloading();
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "reload_loop", false);
		reloadState = RELOAD_WAIT;
		break;

	case RELOAD_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, 0))
		{
			owner->Event_AddToClip(owner->ClipSize());

			if ((ammoClip < clip_size) && (ammoClip < ammoAvail)) {
				reloadState = RELOAD_NOTSET;
			}
			else {
				owner->WeaponState(WP_IDLE, 0);
				reloadState = 0;
			}
		}
		break;
	}
}