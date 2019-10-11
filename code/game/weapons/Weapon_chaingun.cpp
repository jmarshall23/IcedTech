// Weapon_chaingun.cpp
//

#include "precompiled.h"
#include "../Game_local.h"

CLASS_DECLARATION(rvmWeaponObject, rvmWeaponChainGun)
END_CLASS

#define CHAINGUN_FIRE_SKIPFRAMES	7		// 6 shots per second
#define CHAINGUN_LOWAMMO			10
#define CHAINGUN_NUMPROJECTILES		1
#define	CHAINGUN_BARREL_SPEED		( 60 * ( USERCMD_HZ / CHAINGUN_FIRE_SKIPFRAMES ) )
#define	CHAINGUN_BARREL_ACCEL_TIME	0.4
#define CHAINGUN_BARREL_DECCEL_TIME	1.0
#define	CHAINGUN_BARREL_ACCEL		( CHAINGUN_BARREL_SPEED / CHAINGUN_BARREL_ACCEL_TIME )
#define CHAINGUN_BARREL_DECCEL		( CHAINGUN_BARREL_SPEED / CHAINGUN_BARREL_DECCEL_TIME )

// blend times
#define CHAINGUN_IDLE_TO_LOWER		4
#define CHAINGUN_IDLE_TO_FIRE		0
#define	CHAINGUN_IDLE_TO_RELOAD		4
#define CHAINGUN_RAISE_TO_IDLE		0
#define CHAINGUN_WINDDOWN_TO_IDLE	0
#define CHAINGUN_RELOAD_TO_IDLE		0


/*
===============
rvmWeaponChainGun::Init
===============
*/
void rvmWeaponChainGun::Init(idWeapon *weapon) {
	rvmWeaponObject::Init(weapon);

	world_model = (idAnimatedEntity *)owner->GetWorldModel();
	world_barrel_joint = owner->GetAnimator()->GetJointHandle("toob");
	barrel_joint = owner->GetAnimator()->GetJointHandle("spinner");
	barrel_angle = 0;
	current_rate = 0;
	start_rate = 0;
	end_rate = 0;
	spin_start = 0;
	numSkipFrames = 0;
	spin_end = 0;
	spread = weapon->GetFloat("spread"); // weapon->GetFloat("spread")
	weapon->WeaponState(WP_RISING, 0);

	snd_windup = declManager->FindSound("snd_windup");
	snd_winddown = declManager->FindSound("snd_winddown");
}

/*
===============
rvmWeaponChainGun::UpdateBarrel
===============
*/
void rvmWeaponChainGun::UpdateBarrel() {
	float currentTime;
	float t;
	idAngles ang;

	currentTime = gameLocal.time;
	//if (currentTime < spin_end) {
	//	t = (currentTime - spin_start) / (spin_end - spin_start);
	//	current_rate = start_rate + t * (end_rate - start_rate);
	//}
	//else {
		current_rate = end_rate;
	//}

	if (current_rate) {
		barrel_angle = barrel_angle + current_rate * GAME_FRAMETIME;

		ang.pitch = 0;
		ang.yaw = 0;
		ang.roll = barrel_angle;
		owner->Event_SetJointAngle(barrel_joint, JOINTMOD_LOCAL, ang);

		ang.yaw = barrel_angle;
		ang.roll = 0;
		world_model->Event_SetJointAngle(world_barrel_joint, JOINTMOD_LOCAL, ang);
	}
}

/*
===============
rvmWeaponChainGun::SpinUp
===============
*/
void rvmWeaponChainGun::SpinUp() {
	start_rate = current_rate;
	end_rate = CHAINGUN_BARREL_SPEED;
	spin_start = gameLocal.time;
	spin_end = spin_start + (end_rate - current_rate) / CHAINGUN_BARREL_ACCEL;
	owner->Event_StartSound("snd_windup", SND_CHANNEL_BODY3, false);
}

/*
===============
rvmWeaponChainGun::SpinDown
===============
*/
void rvmWeaponChainGun::SpinDown() {
	start_rate = current_rate;
	end_rate = 0;
	spin_start = gameLocal.time;
	spin_end = spin_start + (current_rate - end_rate) / CHAINGUN_BARREL_DECCEL;
	owner->Event_StartSound("snd_winddown", SND_CHANNEL_BODY3, false);
}

/*
===============
rvmWeaponChainGun::Raise
===============
*/
void rvmWeaponChainGun::Raise() {
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
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, CHAINGUN_RAISE_TO_IDLE))
		{
			owner->WeaponState(WP_IDLE, CHAINGUN_RAISE_TO_IDLE);
			risingState = RISING_NOTSET;
			isRisen = true;
		}
		break;
	}
}


/*
===============
rvmWeaponChainGun::Lower
===============
*/
void rvmWeaponChainGun::Lower() {
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
rvmWeaponChainGun::Idle
===============
*/
void rvmWeaponChainGun::Idle() {
	float currentTime;
	float clip_size;

	clip_size = owner->ClipSize();

	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	UpdateBarrel();

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
rvmWeaponChainGun::Fire
===============
*/
void rvmWeaponChainGun::Fire() {
	float ammoClip;
	float currentTime;
	float skip;
	
	ammoClip = owner->AmmoInClip();

	enum FireState
	{
		FIRE_NOTSET = 0,
		FIRE_SPIN1,
		FIRE_FIRE,
		FIRE_SKIPFRAMES
	};

	switch (firingState)
	{
	case FIRE_NOTSET:
		SpinUp();
		UpdateBarrel();
		if(current_rate >= end_rate)
		{
			if(ammoClip > 0)
			{
				firingState = FIRE_SPIN1;
			}
			else
			{
				firingState = 0;
				owner->WeaponState(WP_IDLE, 0);
			}
		}
		break;

	case FIRE_SPIN1:
		owner->Event_StartSound("snd_spin", SND_CHANNEL_BODY3, false);
		firingState = FIRE_FIRE;
		break;

	case FIRE_FIRE:
		owner->Event_LaunchProjectiles(CHAINGUN_NUMPROJECTILES, spread, 0, 1, 1);
		owner->Event_StartSound("snd_fire", SND_CHANNEL_BODY3, false);
		if (ammoClip == CHAINGUN_LOWAMMO) {
			owner->Event_StartSound("snd_lowammo", SND_CHANNEL_ITEM, false);
		}
		numSkipFrames = 0;
		firingState = FIRE_SKIPFRAMES;
		break;

	case FIRE_SKIPFRAMES:
		UpdateBarrel();
		numSkipFrames++;
		if(numSkipFrames >= CHAINGUN_FIRE_SKIPFRAMES)
		{
			if (owner->IsFiring())
			{
				firingState = FIRE_FIRE;
				numSkipFrames = 0;
			}
			else
			{
				firingState = 0;
				owner->WeaponState(WP_IDLE, 0);
				SpinDown();
			}
		}
		break;
	}
}

/*
===============
rvmWeaponChainGun::HasWaitSignal
===============
*/
bool rvmWeaponChainGun::HasWaitSignal(void) {
	if (rvmWeaponObject::HasWaitSignal())
		return true;

	// Allow the player to shoot or reload while reloading.
	return false; // next_attack >= MS2SEC(gameLocal.realClientTime);
}


/*
===============
rvmWeaponChainGun::Reload
===============
*/
void rvmWeaponChainGun::Reload() {
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
		owner->Event_PlayAnim(ANIMCHANNEL_ALL, "reload", false);
		reloadState = RELOAD_WAIT;
		break;

	case RELOAD_WAIT:
		if (owner->Event_AnimDone(ANIMCHANNEL_ALL, 0))
		{
			owner->Event_AddToClip(owner->ClipSize());
			owner->WeaponState(WP_IDLE, CHAINGUN_RELOAD_TO_IDLE);
			reloadState = 0;
		}
		break;
	}
}