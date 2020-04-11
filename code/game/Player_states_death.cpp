// Player_states_death.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

/*
================
idPlayer::State_All_Die
================
*/
stateResult_t idPlayer::State_All_Die(const stateParms_t& parms) {
	animator.PlayAnim(ANIMCHANNEL_ALL, anim_death, gameLocal.time, 0);
	SetAnimState(ANIMCHANNEL_TORSO, "All_Dead", 0, 0);
	return SRESULT_DONE;
}

/*
================
idPlayer::State_All_Dead
================
*/
stateResult_t idPlayer::State_All_Dead(const stateParms_t& parms) {
	return SRESULT_WAIT;
}
