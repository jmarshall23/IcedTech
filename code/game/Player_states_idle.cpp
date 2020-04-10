// Player_states_idle.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

/*
================
idPlayer::State_All_Idle
================
*/
stateResult_t idPlayer::State_All_Idle(const stateParms_t& parms) {
	if (pfl.crouch) {
		if (pfl.forward || pfl.backward) {
			SetAnimState(ANIMCHANNEL_TORSO, "All_RunCrouch", 0, 0);
		}
		else {
			SetAnimState(ANIMCHANNEL_TORSO, "All_IdleCrouch", 0, 0);
		}
		return SRESULT_DONE;
	}

	if (SRESULT_WAIT == State_All_IdleThink(parms)) {
		animator.CycleAnim(ANIMCHANNEL_ALL, anim_idle, gameLocal.time, 0);
		SetAnimState(ANIMCHANNEL_TORSO, "All_IdleThink", 0, 0);
	}
	return SRESULT_DONE;
}

/*
================
idPlayer::State_All_IdleThink
================
*/
stateResult_t idPlayer::State_All_IdleThink(const stateParms_t& parms) {
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

	if (pfl.forward || pfl.backward) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Run", 0, 0);
		return SRESULT_DONE;

	}

	return SRESULT_WAIT;
}

/*
================
idPlayer::State_All_IdleCrouch
================
*/
stateResult_t idPlayer::State_All_IdleCrouch(const stateParms_t& parms) {
	if (!pfl.crouch) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		return SRESULT_DONE;
	}

	if (SRESULT_WAIT == State_All_IdleCrouchThink(parms)) {
		animator.CycleAnim(ANIMCHANNEL_ALL, anim_idle_crouch, gameLocal.time, 0);
		SetAnimState(ANIMCHANNEL_TORSO, "All_IdleCrouchThink", 0, 0);
	}
	return SRESULT_DONE;
}

/*
================
idPlayer::State_All_IdleCrouchThink
================
*/
stateResult_t idPlayer::State_All_IdleCrouchThink(const stateParms_t& parms) {
	if (!pfl.crouch) {
		if (pfl.forward || pfl.backward) {
			SetAnimState(ANIMCHANNEL_TORSO, "All_Run", 0, 0);
		}
		else {
			SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		}
		return SRESULT_DONE;
	}

	if (pfl.forward || pfl.backward) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_RunCrouch", 0, 0);
		return SRESULT_DONE;

	}

	return SRESULT_WAIT;
}
