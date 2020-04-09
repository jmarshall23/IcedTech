// Player_states.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

CLASS_STATES_DECLARATION(idPlayer)
	// Animation States
	STATE("All_Idle", idPlayer::State_All_Idle)
	STATE("All_IdleThink", idPlayer::State_All_IdleThink)

	STATE("All_Walk", idPlayer::State_All_Walk)
	STATE("All_WalkThink", idPlayer::State_All_WalkThink)
END_CLASS_STATES

/*
================
idPlayer::State_All_Idle
================
*/
stateResult_t idPlayer::State_All_Idle(const stateParms_t& parms) {
	if (SRESULT_WAIT == State_All_IdleThink(parms)) {
		animator.CycleAnim(ANIMCHANNEL_ALL, anim_idle, gameLocal.time, 0);
		SetAnimState(ANIMCHANNEL_TORSO, "All_IdleThink", 0, 0);		
	}
	return SRESULT_DONE;
}

/*
================
idPlayer::State_All_Walk
================
*/
stateResult_t idPlayer::State_All_Walk(const stateParms_t& parms) {
	if (!(pfl.forward || pfl.backward)) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		return SRESULT_DONE;
	}
	animator.CycleAnim(ANIMCHANNEL_ALL, anim_run, gameLocal.time, 0);
	SetAnimState(ANIMCHANNEL_TORSO, "All_WalkThink", 0, 0);
	return SRESULT_WAIT;
}

/*
================
idPlayer::State_All_WalkThink
================
*/
stateResult_t idPlayer::State_All_WalkThink(const stateParms_t& parms) {
	if (!(pfl.forward || pfl.backward)) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Idle", 0, 0);
		return SRESULT_DONE;

	}
	return SRESULT_WAIT;
}

/*
================
idPlayer::State_All_IdleThink
================
*/
stateResult_t idPlayer::State_All_IdleThink(const stateParms_t& parms) {
	if (pfl.forward || pfl.backward) {
		SetAnimState(ANIMCHANNEL_TORSO, "All_Walk", 0, 0);
		return SRESULT_DONE;
		
	}
	//if (usercmd.buttons & BUTTON_RUN) {
	//	PlayCycle(ANIMCHANNEL_LEGS, "run_forward", parms.blendFrames);
	//	PostAnimState(ANIMCHANNEL_LEGS, "Legs_Run_Forward", parms.blendFrames);
	//}
	//else {
	//	PlayCycle(ANIMCHANNEL_LEGS, "walk_forward", parms.blendFrames);
	//	PostAnimState(ANIMCHANNEL_LEGS, "Legs_Walk_Forward", parms.blendFrames);
	//}

	//if (pfl.teleport) {
	//	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Teleport", 0, 0, 0);
	//	return SRESULT_DONE;
	//}
	//if (pfl.weaponFired) {
	//	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Fire", 0, 0, 0);
	//	return SRESULT_DONE;
	//}
	//
	//if (pfl.attackHeld && weapon && weapon->wfl.hasWindupAnim) {
	//	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Fire_Windup", 0, 0, 0);
	//	return SRESULT_DONE;
	//}
	//
	//if (pfl.attackHeld && HasAnim(ANIMCHANNEL_TORSO, "startfire")) {
	//	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Fire_StartFire", 2, 0, 0);
	//	return SRESULT_DONE;
	//}
	//if (pfl.pain) {
	//	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Pain", 0, 0, 0);
	//	return SRESULT_DONE;
	//}
	//if (emote != PE_NONE) {
	//	PostAnimState(ANIMCHANNEL_TORSO, "Torso_Emote", 5, 0, 0);
	//	return SRESULT_DONE;
	//}
	
	return SRESULT_WAIT;
}
