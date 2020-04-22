// BotAI_Battle_Fight.cpp
//

#include "game_precompiled.h"
#include "../../Game_local.h"

rvmBotAIBotBattleFight botAIBattleFight;

/*
=====================
rvmBotAIBotBattleFight::Think
=====================
*/
void rvmBotAIBotBattleFight::Think(bot_state_t* bs) {
	int areanum;
	idVec3 target;
	idPlayer* entinfo;

	// respawn if dead.
	if (BotIsDead(bs)) {
		bs->action = &botAIRespawn;
		return;
	}

	//if there is another better enemy
	if (BotFindEnemy(bs, bs->enemy)) {
		common->DPrintf("found new better enemy\n");
	}

	//if no enemy
	if (bs->enemy < 0) {
		bs->action = &botAIActionSeekLTG;
		return;
	}

	//BotEntityInfo(bs->enemy, &entinfo);
	entinfo = gameLocal.entities[bs->enemy]->Cast<idPlayer>();// &g_entities[bs->enemy];

	//if the enemy is dead
	if (bs->enemydeath_time) {
		if (bs->enemydeath_time < Bot_Time() - 1.0) {
			bs->enemydeath_time = 0;
			if (bs->enemysuicide) {
				// jmarshall - bot chat
								//BotChat_EnemySuicide(bs);
				// jmarshall end
			}
			// jmarshall - bot chat stand
			//			if (bs->lastkilledplayer == bs->enemy && BotChat_Kill(bs)) {
			//				bs->stand_time = FloatTime() + BotChatTime(bs);
			//				AIEnter_Stand(bs, "battle fight: enemy dead");
			//			}
			//			else {
			bs->ltg_time = 0;
			//AIEnter_Seek_LTG(bs, "battle fight: enemy dead");
			bs->action = &botAIActionSeekLTG;
			//			}
			// jmarshall end
			return;
		}
	}
	else {
		if (EntityIsDead(entinfo)) {
			bs->enemydeath_time = Bot_Time();
		}
	}
	//if the enemy is invisible and not shooting the bot looses track easily
	if (entinfo->IsInvisible() && !entinfo->IsShooting()) {
		if (rvmBotUtil::random() < 0.2) {
			bs->action = &botAIActionSeekLTG;
			return;
		}
	}
	//
	target = entinfo->GetOrigin();

	//update the reachability area and origin if possible
	//areanum = BotPointAreaNum(target);
	//if (areanum && trap_AAS_AreaReachability(areanum)) {
	//	VectorCopy(target, bs->lastenemyorigin);
	//	bs->lastenemyareanum = areanum;
	//}
//	bs->lastenemyareanum = areanum;

	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);

	//if the bot's health decreased
// jmarshall - bot chat
	//if (bs->lastframe_health > bs->inventory[INVENTORY_HEALTH]) {
	//	if (BotChat_HitNoDeath(bs)) {
	//		bs->stand_time = FloatTime() + BotChatTime(bs);
	//		AIEnter_Stand(bs, "battle fight: chat health decreased");
	//		return qfalse;
	//	}
	//}

	//if the bot hit someone
	//if (bs->cur_ps.persistant[PERS_HITS] > bs->lasthitcount) {
	//	if (BotChat_HitNoKill(bs)) {
	//		bs->stand_time = FloatTime() + BotChatTime(bs);
	//		AIEnter_Stand(bs, "battle fight: chat hit someone");
	//		return qfalse;
	//	}
	//}
// jmarshall end

	//if the enemy is not visible
	if (!BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
		if (BotWantsToChase(bs)) {
			//AIEnter_Battle_Chase(bs, "battle fight: enemy out of sight");
			bs->action = &botAIBattleChase;
			return;
		}
		else {
			//AIEnter_Seek_LTG(bs, "battle fight: enemy out of sight");
			bs->action = &botAIActionSeekLTG;
			return;
		}
	}
	//use holdable items
	BotBattleUseItems(bs);
	//
	//bs->tfl = TFL_DEFAULT;
	//if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	////if in lava or slime the bot should be able to get out
	//if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA | TFL_SLIME;
	////
	//if (BotCanAndWantsToRocketJump(bs)) {
	//	bs->tfl |= TFL_ROCKETJUMP;
	//}
	//choose the best weapon to fight with
	BotChooseWeapon(bs);
	//do attack movements
	BotAttackMove(bs, 0);

	//if the movement failed
	//if (moveresult.failure) {
	//	//reset the avoid reach, otherwise bot is stuck in current area
	//	trap_BotResetAvoidReach(bs->ms);
	//	//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
	//	bs->ltg_time = 0;
	//}
	//BotAIBlocked(bs, &moveresult, qfalse);

	//aim at the enemy
	BotAimAtEnemy(bs);

	//attack the enemy if possible
	BotCheckAttack(bs);

	//if the bot wants to retreat
	if (!(bs->flags & BFL_FIGHTSUICIDAL)) {
		if (BotWantsToRetreat(bs)) {
			//AIEnter_Battle_Retreat(bs, "battle fight: wants to retreat");
			bs->action = &botAIBattleRetreat;
			return;
		}
	}
}