// BotAI_respawn.cpp
//

#include "Game_precompiled.h"
#include "../../Game_local.h"

rvmBotAIBotRespawn botAIRespawn;

/*
=====================
rvmBotAIBotRespawn::Think
=====================
*/
void rvmBotAIBotRespawn::Think(bot_state_t* bs) {
	if(!BotIsDead(bs)) {
		bs->botinput.respawn = false;
		bs->action = &botAIActionSeekLTG;
		return;
	}

	bs->botinput.respawn = true;
}