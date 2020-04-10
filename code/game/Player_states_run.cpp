// Player_states_run.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

/*
================
idPlayer::State_All_Run
================
*/
stateResult_t idPlayer::State_All_Run(const stateParms_t& parms) {
	if (pfl.crouch) {
		if (pfl.forward || pfl.backward)
		{
			SetAnimState(ANIMCHANNEL_TORSO, "All_RunCrouch", 0, 0);
		}
		else
		{
			SetAnimState(ANIMCHANNEL_TORSO, "All_IdleCrouch", 0, 0);
		}
		return SRESULT_DONE;
	}

	if (!(pfl.forward || pfl.backward)) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		return SRESULT_DONE;
	}
	animator.CycleAnim(ANIMCHANNEL_ALL, anim_run, gameLocal.time, 0);
	SetAnimState(ANIMCHANNEL_TORSO, "All_RunThink", 0, 0);
	return SRESULT_WAIT;
}

/*
================
idPlayer::State_All_RunThink
================
*/
stateResult_t idPlayer::State_All_RunThink(const stateParms_t& parms) {
	if (pfl.crouch) {
		if (pfl.forward || pfl.backward)
		{
			SetAnimState(ANIMCHANNEL_TORSO, "All_RunCrouch", 0, 0);
		}
		else
		{
			SetAnimState(ANIMCHANNEL_TORSO, "All_IdleCrouch", 0, 0);
		}
		return SRESULT_DONE;
	}

	if (!(pfl.forward || pfl.backward)) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		return SRESULT_DONE;

	}
	return SRESULT_WAIT;
}

/*
================
idPlayer::State_All_RunCrouch
================
*/
stateResult_t idPlayer::State_All_RunCrouch(const stateParms_t& parms) {
	if (!pfl.crouch) {
		if (pfl.forward || pfl.backward) {
			SetAnimState(ANIMCHANNEL_TORSO, "All_Run", 0, 0);
		}
		else {
			SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		}
		return SRESULT_DONE;
	}


	if (!(pfl.forward || pfl.backward)) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_IdleCrouch", 0, 0);
		return SRESULT_DONE;
	}
	animator.CycleAnim(ANIMCHANNEL_ALL, anim_walk_crouch, gameLocal.time, 0);
	SetAnimState(ANIMCHANNEL_TORSO, "All_RunCrouchThink", 0, 0);
	return SRESULT_WAIT;
}

/*
================
idPlayer::State_All_RunThink
================
*/
stateResult_t idPlayer::State_All_RunCrouchThink(const stateParms_t& parms) {
	if (!pfl.crouch) {
		if (pfl.forward || pfl.backward) {
			SetAnimState(ANIMCHANNEL_TORSO, "All_Run", 0, 0);
		}
		else {
			SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		}
		return SRESULT_DONE;
	}

	if (!(pfl.forward || pfl.backward)) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_IdleCrouch", 0, 0);
		return SRESULT_DONE;

	}
	return SRESULT_WAIT;
}
