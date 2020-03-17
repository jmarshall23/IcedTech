// BotAI_Battle_Retreat.cpp
//

#include "game_precompiled.h"
#include "../../Game_local.h"

rvmBotAIBotBattleRetreat botAIBattleRetreat;

/*
=====================
rvmBotAIBotBattleRetreat::Think
=====================
*/
void rvmBotAIBotBattleRetreat::Think(bot_state_t* bs) {
	bot_goal_t goal;
	idPlayer* entinfo;
	idVec3 target, dir;
	float attack_skill, range;

	// respawn if dead.
	if (BotIsDead(bs)) {
		bs->action = &botAIRespawn;
		return;
	}

	// if no enemy.
	if(bs->enemy < 0) {
		bs->action = &botAIActionSeekLTG;
		return;
	}

	// Ensure the target is a player.
	entinfo = gameLocal.entities[bs->enemy]->Cast<idPlayer>();
	if(!entinfo) {
		bs->action = &botAIActionSeekLTG;
		return;
	}

	// If our enemy is dead, search for another LTG.
	if(EntityIsDead(entinfo)) {
		bs->action = &botAIActionSeekLTG;
		return;
	}

	//if there is another better enemy
	if(BotFindEnemy(bs, bs->enemy)) {
		common->DPrintf("found new better enemy\n");
	}

	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);

	//if the bot doesn't want to retreat anymore... probably picked up some nice items
	if (BotWantsToChase(bs)) {
		//empty the goal stack, when chasing, only the enemy is the goal
		botGoalManager.BotEmptyGoalStack(bs->gs);

		//go chase the enemy
		//AIEnter_Battle_Chase(bs, "battle retreat: wants to chase");
		bs->action = &botAIBattleChase;
		return;
	}

	//update the last time the enemy was visible
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
		bs->enemyvisible_time = Bot_Time();
		target = entinfo->GetOrigin();
		bs->lastenemyorigin = target;
	}

	//if the enemy is NOT visible for 4 seconds
	if (bs->enemyvisible_time < Bot_Time() - 4) {
		bs->action = &botAIActionSeekLTG;
		return;
	}
	//else if the enemy is NOT visible
	else if (bs->enemyvisible_time < Bot_Time()) {
		//if there is another enemy
		if (BotFindEnemy(bs, -1)) {
			//AIEnter_Battle_Fight(bs, "battle retreat: another enemy");
			bs->action = &botAIBattleFight;
			return;
		}
	}

	//use holdable items
	BotBattleUseItems(bs);

	//get the current long term goal while retreating
	if (!BotGetItemLongTermGoal(bs, 0, &bs->currentGoal)) {
		//AIEnter_Battle_SuicidalFight(bs, "battle retreat: no way out");
		bs->action = &botAIBattleFight;
		bs->flags |= BFL_FIGHTSUICIDAL;
		return;
	}

	//check for nearby goals periodicly
	if (bs->check_time < Bot_Time()) {
		bs->check_time = Bot_Time() + 1;
		range = 150;

		//
		if (BotNearbyGoal(bs, 0, &goal, range)) {
			//trap_BotResetLastAvoidReach(bs->ms);
			//time the bot gets to pick up the nearby goal item
			bs->nbg_time = Bot_Time() + range / 100 + 1;
			//AIEnter_Battle_NBG(bs, "battle retreat: nbg");
			bs->action = &botAIBattleNBG;
			return;
		}
	}

	if (idMath::FRandRange(0.0f, 4.0f) <= 2.0f)
	{
		//BotMoveToGoal(bs, &goal);
		bs->currentGoal = goal;
	}
	else
	{
		BotMoveInRandomDirection(bs);
	}

	BotChooseWeapon(bs);

	BotAimAtEnemy(bs);

	//attack the enemy if possible
	BotCheckAttack(bs);
}