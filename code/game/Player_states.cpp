// Player_states.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

CLASS_STATES_DECLARATION(idPlayer)
	// Animation States
	STATE("All_Idle", idPlayer::State_All_Idle)
	STATE("All_IdleThink", idPlayer::State_All_IdleThink)

	STATE("All_IdleCrouch", idPlayer::State_All_IdleCrouch)
	STATE("All_IdleCrouchThink", idPlayer::State_All_IdleCrouchThink)

	STATE("All_Run", idPlayer::State_All_Run)
	STATE("All_RunThink", idPlayer::State_All_RunThink)

	STATE("All_RunCrouch", idPlayer::State_All_RunCrouch)
	STATE("All_RunCrouchThink", idPlayer::State_All_RunCrouchThink)

	STATE("All_Die", idPlayer::State_All_Die)
	STATE("All_Dead", idPlayer::State_All_Dead)
END_CLASS_STATES

