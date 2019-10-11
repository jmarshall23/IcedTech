// Weapon_flashlight.cpp
//

#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponFlashlight)
END_CLASS

// blend times
#define FLASHLIGHT_IDLE_TO_LOWER	4
#define FLASHLIGHT_IDLE_TO_FIRE		2
#define	FLASHLIGHT_IDLE_TO_RELOAD	4
#define FLASHLIGHT_RAISE_TO_IDLE	4
#define FLASHLIGHT_FIRE_TO_IDLE		4
#define FLASHLIGHT_RELOAD_TO_IDLE	4

#define	FLASHLIGHT_MIN_SKIN_INTENSITY	0.2

/*
================
rvmWeaponFlashlight::Init
================
*/
void rvmWeaponFlashlight::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	skin_on = owner->GetKey("skin_on");
	skin_on_invis = owner->GetKey("skin_on_invis");
	skin_off = owner->GetKey("skin_off");
	skin_off_invis = owner->GetKey("skin_off_invis");

	intensity = 1.0;

	owner->Event_SetLightParm(3, 1.0);
	owner->Event_SetShaderParm(3, 1.0);

	on = true;

	UpdateSkin();

	weapon->WeaponState(WP_RISING, 0);
}

/*
================
rvmWeaponFlashlight::UpdateLightIntensity
================
*/
void rvmWeaponFlashlight::UpdateLightIntensity(void) {
	// TODO this has to interact with scripts somehow
}

/*
================
rvmWeaponFlashlight::UpdateSkin
================
*/
void rvmWeaponFlashlight::UpdateSkin(void) {
	if (on && (intensity > FLASHLIGHT_MIN_SKIN_INTENSITY)) {
		if (!owner->Event_IsInvisible()) {
			owner->Event_SetSkin(skin_on);
		}
		else {
			owner->Event_SetSkin(skin_on_invis);
		}
	}
	else {
		if (!owner->Event_IsInvisible()) {
			owner->Event_SetSkin(skin_off);
		}
		else {
			owner->Event_SetSkin(skin_off_invis);
		}
	}
}

/*
================
rvmWeaponFlashlight::Raise
================
*/
void rvmWeaponFlashlight::Raise(void) {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, FLASHLIGHT_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, FLASHLIGHT_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
		}
		break;
	}
}

/*
================
rvmWeaponFlashlight::Lower
================
*/
void rvmWeaponFlashlight::Lower() {
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
================
rvmWeaponFlashlight::Idle
================
*/
void rvmWeaponFlashlight::Idle() {
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
================
rvmWeaponFlashlight::Fire
================
*/
void rvmWeaponFlashlight::Fire() {
	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_MELEE,
		FIRE_WAIT
	};

	switch (firingState)
	{
	case FIRE_NOTSET:
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "fire", false);
		firingState = FIRE_MELEE;
		Wait(0.1f);
		break;

	case FIRE_MELEE:
		owner->Event_Melee();
		firingState = FIRE_WAIT;
		break;

	case FIRE_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, FLASHLIGHT_FIRE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, FLASHLIGHT_FIRE_TO_IDLE);
			firingState = 0;
		}
		break;
	}
}

/*
================
rvmWeaponFlashlight::Reload
================
*/
void rvmWeaponFlashlight::Reload() {
	enum RELOAD_State
	{
		RELOAD_NOTSET = 0,
		RELOAD_TOGGLEFLASHLIGHT,
		RELOAD_WAIT
	};

	switch (reloadState)
	{
	case RELOAD_NOTSET:
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "reload", false);
		reloadState = RELOAD_TOGGLEFLASHLIGHT;
		Wait(0.2);
		break;

	case RELOAD_TOGGLEFLASHLIGHT:
		on = !on;
		UpdateSkin();
		owner->Event_Flashlight(on);
		reloadState = RELOAD_WAIT;
		break;

	case RELOAD_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, FLASHLIGHT_RELOAD_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, FLASHLIGHT_RELOAD_TO_IDLE);
			reloadState = 0;
		}
		break;
	}
}
