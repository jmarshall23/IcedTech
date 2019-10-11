// Weapon_pda.cpp
//

#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponPDA)
END_CLASS


/*
================
rvmWeaponPDA::Init
================
*/
void rvmWeaponPDA::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	weapon->WeaponState(WP_RISING, 0);
}

/*
================
rvmWeaponPDA::Raise
================
*/
void rvmWeaponPDA::Raise(void) {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, 0))
		{
			owner->WeaponState(WP_IDLE, 0);
			risingState = RISING_NOTSET;
			isRisen = true;
		}
		break;
	}
}

/*
================
rvmWeaponPDA::Lower
================
*/
void rvmWeaponPDA::Lower() {
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
		owner->GetOwner()->SelectWeapon(owner->GetOwner()->GetPrevWeapon(), true);
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
rvmWeaponPDA::Idle
================
*/
void rvmWeaponPDA::Idle() {
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch (idleState)
	{
	case IDLE_NOTSET:
		owner->GetOwner()->Event_OpenPDA();
		idleState = IDLE_WAIT;
		break;

	case IDLE_WAIT:
		if(!owner->GetOwner()->objectiveSystemOpen)
		{
			owner->WeaponState(WP_LOWERING, 0);
		}
		break;
	}
}

/*
================
rvmWeaponPDA::Fire
================
*/
void rvmWeaponPDA::Fire() {
	owner->WeaponState(WP_IDLE, 0);
}

/*
================
rvmWeaponPDA::Reload
================
*/
void rvmWeaponPDA::Reload() {

}
