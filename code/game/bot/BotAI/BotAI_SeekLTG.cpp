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

	// No Enemy.
	bs->enemy = -1;

	//if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			//keep the current long term goal and retreat
			//AIEnter_Battle_Retreat(bs, "seek ltg: found enemy");
			bs->action = &botAIBattleRetreat;
			return;
		}
		else {
			//trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			botGoalManager.BotEmptyGoalStack(bs->gs);

			//go fight
			bs->action = &botAIBattleFight;
			return;
		}
	}
}
