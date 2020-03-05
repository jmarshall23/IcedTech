// BotAI.cpp
//

#include "game_precompiled.h"
#include "../../Game_local.h"

/*
=========================
rvmBotAIBotActionBase::BotIsDead
=========================
*/
bool rvmBotAIBotActionBase::BotIsDead(bot_state_t* bs) {
	idPlayer* player = gameLocal.GetClientByNum(bs->client);
	if (player->health <= 0)
		return true;

	return false;
}