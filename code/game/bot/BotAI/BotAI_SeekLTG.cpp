// BotAI_SeekLTG.cpp
//

#include "game_precompiled.h"
#include "../../Game_local.h"

rvmBotAIBotSeekLTG botAIActionSeekLTG;

/*
=====================
rvmBotAIBotSeekLTG::Think
=====================
*/
void rvmBotAIBotSeekLTG::Think(bot_state_t* bs) {
	if(BotIsDead(bs)) {
		bs->action = &botAIRespawn;
		return;
	}

	if (!BotGetItemLongTermGoal(bs, 0, &bs->currentGoal)) {
		return;
	}
}
