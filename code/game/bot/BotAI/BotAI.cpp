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

/*
==================
rvmBotAIBotActionBase::BotReachedGoal
==================
*/
bool rvmBotAIBotActionBase::BotReachedGoal(bot_state_t* bs, bot_goal_t* goal) {
	if (goal->flags & GFL_ITEM) {
		//if touching the goal
		if (botGoalManager.BotTouchingGoal(bs->origin, goal)) {
			if (!(goal->flags & GFL_DROPPED)) {
				botGoalManager.BotSetAvoidGoalTime(bs->gs, goal->number, -1);
			}
			return true;
		}
		//if the goal isn't there
		if (botGoalManager.BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal)) {
			return true;
		}
	} else {
		//if touching the goal
		if (botGoalManager.BotTouchingGoal(bs->origin, goal))
			return true;
	}
	return false;
}

/*
==================
rvmBotAIBotActionBase::BotChooseWeapon
==================
*/
void rvmBotAIBotActionBase::BotChooseWeapon(bot_state_t* bs) {
	int newweaponnum;

	//if (bs->cur_ps.weaponstate == WEAPON_RAISING ||
	//	bs->cur_ps.weaponstate == WEAPON_DROPPING) {
	//	//trap_EA_SelectWeapon(bs->client, bs->weaponnum);
	//	bs->input.weapon = bs->weaponnum;
	//}
	//else {
		newweaponnum = botWeaponInfoManager.BotChooseBestFightWeapon(bs->ws, bs->inventory);
		if (bs->weaponnum != newweaponnum) 
			bs->weaponchange_time = Bot_Time();
		bs->weaponnum = newweaponnum;
		//BotAI_Print(PRT_MESSAGE, "bs->weaponnum = %d\n", bs->weaponnum);
		//trap_EA_SelectWeapon(bs->client, bs->weaponnum);
		bs->botinput.weapon = bs->weaponnum;
	//}
}


/*
==================
rvmBotAIBotActionBase::BotGetItemLongTermGoal
==================
*/
int rvmBotAIBotActionBase::BotGetItemLongTermGoal(bot_state_t* bs, int tfl, bot_goal_t* goal) {
	//if the bot has no goal
	if (!botGoalManager.BotGetTopGoal(bs->gs, goal)) {
		//BotAI_Print(PRT_MESSAGE, "no ltg on stack\n");
		bs->ltg_time = 0;
	}
	//if the bot touches the current goal
	else if (BotReachedGoal(bs, goal)) {
		BotChooseWeapon(bs);
		bs->ltg_time = 0;
	}

	// Check to see that we can get to our goal, if not get a new goal.
	//if (bs->numMovementWaypoints > 0)
	//{
	//	trace_t tr;
	//	gentity_t* ent = &g_entities[bs->client];
	//	vec3_t waypoint;
	//
	//	VectorCopy(bs->movement_waypoints[bs->currentWaypoint], waypoint);
	//	waypoint[2] += 5.0f;
	//
	//	trap_Trace(&tr, ent->r.currentOrigin, NULL, NULL, waypoint, bs->client, CONTENTS_SOLID);
	//
	//	if (tr.fraction <= 0.7f)
	//	{
	//		BotChooseWeapon(bs);
	//		bs->ltg_time = 0;
	//	}
	//}

	//if it is time to find a new long term goal
	if (bs->ltg_time == 0) {
		//pop the current goal from the stack
		botGoalManager.BotPopGoal(bs->gs);		
		//BotAI_Print(PRT_MESSAGE, "%s: choosing new ltg\n", ClientName(bs->client, netname, sizeof(netname)));
		//choose a new goal
		//BotAI_Print(PRT_MESSAGE, "%6.1f client %d: BotChooseLTGItem\n", FloatTime(), bs->client);
		if (botGoalManager.BotChooseLTGItem(bs->gs, bs->origin, bs->inventory, tfl)) {
			char buf[128];
			//get the goal at the top of the stack
			botGoalManager.BotGetTopGoal(bs->gs, goal);
			botGoalManager.BotGoalName(goal->number, buf, sizeof(buf));
			common->Printf("%1.1f: new long term goal %s\n", Bot_Time(), buf);

			bs->ltg_time = Bot_Time() + 20;
			bs->currentGoal.framenum = gameLocal.framenum;
		}
		else {//the bot gets sorta stuck with all the avoid timings, shouldn't happen though
			//
			//trap_BotDumpAvoidGoals(bs->gs);
			//reset the avoid goals and the avoid reach
			botGoalManager.BotResetAvoidGoals(bs->gs);
			//BotResetAvoidReach(bs->ms);
		}
		//get the goal at the top of the stack
		if (!botGoalManager.BotGetTopGoal(bs->gs, goal))
			return false;

		bs->currentGoal.framenum = gameLocal.framenum;

		return true;
	}
	return true;
}