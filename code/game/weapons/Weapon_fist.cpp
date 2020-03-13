// weapon_fist.cpp
//

#include "Game_precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponFist)
END_CLASS

#define RIFLE_NUMPROJECTILES	1

// blend times
#define FISTS_IDLE_TO_LOWER		4
#define FISTS_IDLE_TO_PUNCH		0
#define FISTS_RAISE_TO_IDLE		4
#define FISTS_PUNCH_TO_IDLE		1


/*
================
rvmWeaponFist::Init
================
*/
void rvmWeaponFist::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	weapon->WeaponState(WP_RISING, 0);
	owner->Event_WeaponReady();
}

/*
================
rvmWeaponFist::Raise
================
*/
void rvmWeaponFist::Raise(void) {
	enum RisingState
	{
		RISING_NOTSET = 0,
		RISING_WAIT
	};

	switch(risingState)
	{
		case RISING_NOTSET:
			owner->Event_WeaponRising();
			owner->Event_PlayAnim(ANIMCHANNEL_ALL, "raise", false);
			risingState = RISING_WAIT;
			break;

		case RISING_WAIT:			
			if(owner->Event_AnimDone(ANIMCHANNEL_ALL, FISTS_RAISE_TO_IDLE))
			{
				owner->WeaponState(WP_IDLE, FISTS_RAISE_TO_IDLE);
				risingState = RISING_NOTSET;
				isRisen = true;
			}
			break;
	}
}

/*
================
rvmWeaponFist::Lower
================
*/
void rvmWeaponFist::Lower() {
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
================
rvmWeaponFist::Idle
================
*/
void rvmWeaponFist::Idle() {
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch(idleState)
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
rvmWeaponFist::Fire
================
*/
void rvmWeaponFist::Fire() {
	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_MELEE,
		FIRE_WAIT
	};

	switch(firingState)
	{
		case FIRE_NOTSET:
			owner->Event_PlayAnim(ANIMCHANNEL_ALL, GetFireAnim(), false);
			firingState = FIRE_MELEE;
			Wait(0.1f);
			break;

		case FIRE_MELEE:
			owner->Event_Melee();
			firingState = FIRE_WAIT;
			break;

		case FIRE_WAIT:
			if (owner->Event_AnimDone(ANIMCHANNEL_ALL, 0))
			{
				side = !side;
				owner->WeaponState(WP_IDLE, FISTS_PUNCH_TO_IDLE);
				firingState = 0;
			}
			break;
	}
}

/*
================
rvmWeaponFist::Reload
================
*/
void rvmWeaponFist::Reload() {
	
}

/*
================
rvmWeaponFist::GetFireAnim
================
*/
const char *rvmWeaponFist::GetFireAnim() {
	if (side) {
		return "punch_left";
	}
	else {
		return "punch_right";
	}
}