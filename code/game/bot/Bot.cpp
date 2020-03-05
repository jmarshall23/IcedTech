// Bot.cpp
//

#include "game_precompiled.h"
#include "../Game_local.h"

idCVar bot_pathdebug("bot_pathdebug", "0", CVAR_BOOL | CVAR_CHEAT, "force the bot to path to player");
idCVar bot_goaldist("bot_goaldist", "20", CVAR_INTEGER | CVAR_CHEAT, "");
idCVar bot_debugnav("bot_debugnav", "0", CVAR_BOOL | CVAR_CHEAT, "draws navmesh paths for the bot");

CLASS_DECLARATION(idPlayer, rvmBot)
END_CLASS

/*
===================
rvmBot::rvmBot
===================
*/
rvmBot::rvmBot() {
	botAction = NULL;
	currentWaypoint = BOT_NOWAYPOINT;
	gameLocal.RegisterBot(this);
}

/*
===================
rvmBot::~rvmBot
===================
*/
rvmBot::~rvmBot() {
	gameLocal.UnRegisterBot(this);
}

/*
===================
rvmBot::Spawn
===================
*/
void rvmBot::Spawn(void) {	
	idStr botName = spawnArgs.GetString("botname");
	char filename[256];
	int errnum;

	// Load in the bot character.
	bs.character = botCharacterStatsManager.BotLoadCharacterFromFile(va("bots/%s_c.c", botName.c_str()), 1);
	if(!bs.character) {
		gameLocal.Error("Failed to load character file for bot %s\n", botName.c_str());
	}

	// Allocate the goal state.
	bs.gs = botGoalManager.BotAllocGoalState(entityNumber);

	// Get the bot items weights file name.
	botCharacterStatsManager.Characteristic_String(bs.character, CHARACTERISTIC_ITEMWEIGHTS, filename, 256);
	errnum = botGoalManager.BotLoadItemWeights(bs.gs, filename);
	if (errnum != BLERR_NOERROR) {
		gameLocal.Error("Failed to load bot item weights!");
		botGoalManager.BotFreeGoalState(bs.gs);
		return;
	}

	//allocate a weapon state
	bs.ws = botWeaponInfoManager.BotAllocWeaponState();

	//load the weapon weights
	botCharacterStatsManager.Characteristic_String(bs.character, CHARACTERISTIC_WEAPONWEIGHTS, filename, 256);
	errnum = botWeaponInfoManager.BotLoadWeaponWeights(bs.ws, filename);
	if (errnum != BLERR_NOERROR) {
		//	trap_BotFreeGoalState(bs->gs);
		botWeaponInfoManager.BotFreeWeaponState(bs.ws);
		return;
	}

	bs.client = entityNumber;
	bs.entitynum = entityNumber;
	bs.setupcount = 4;
	bs.entergame_time = Bot_Time();

	idPlayer::Spawn();
}

/*
===================
rvmBot::Think
===================
*/
void rvmBot::BotMoveToGoalOrigin(void) {
	botinput.dir = currentGoal.nextMoveOrigin - GetPhysics()->GetOrigin();

	idAngles ang(0, botinput.dir.ToYaw(), 0);
	botinput.viewangles = ang;
	botinput.speed = 200;

	botinput.dir.Normalize();
}

/*
===================
rvmBot::ServerThink
===================
*/
void rvmBot::ServerThink(void) {
	if (bot_pathdebug.IsModified())
	{
		if (bot_pathdebug.GetBool())
		{
			currentGoal.origin = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin();
			currentGoal.framenum = gameLocal.framenum;
		}

		bot_pathdebug.ClearModified();
		bot_pathdebug.SetBool(false);
	}

	// If we have a new goal position, let's figure out how to get there.
	if (currentGoal.framenum == gameLocal.framenum) {
		if (gameLocal.NavGetPathBetweenPoints(GetPhysics()->GetOrigin(), currentGoal.origin, navWaypoints))
		{
			currentWaypoint = 0;
		}
		else
		{
			currentWaypoint = BOT_NOWAYPOINT;
		}
	}

	if(botAction == NULL) {
		botAction = &botAIActionSeekLTG;
	}

	botAction->Think(&bs);

	// If we are moving along a set of waypoints, let's move along.
	if (currentWaypoint != BOT_NOWAYPOINT)
	{
		if (currentWaypoint < navWaypoints.Num())
		{
			float dist = idMath::Distance(GetPhysics()->GetOrigin(), navWaypoints[currentWaypoint]);

			if (dist <= bot_goaldist.GetInteger())
			{
				currentWaypoint++;
				if (currentWaypoint >= navWaypoints.Num())
					currentWaypoint = navWaypoints.Num() - 1;
			}
		}

		currentGoal.nextMoveOrigin = navWaypoints[currentWaypoint];

		BotMoveToGoalOrigin();


		if (bot_debugnav.GetBool() && navWaypoints.Num() > 1)
		{
			if (currentWaypoint < navWaypoints.Num())
			{
				for (int i = currentWaypoint + 1; i < navWaypoints.Num(); i++)
				{
					idVec4 color_white(255, 255, 255, 255);
					gameRenderWorld->DebugArrow(color_white, navWaypoints[i - 1], navWaypoints[i], 10);
				}
			}

			idVec4 color_red(255, 0, 0, 255);
			idVec3 box_debug_extents(20, 20, 20);
			idMat3 rotation;

			rotation.Identity();

			idBox start_box(GetPhysics()->GetOrigin(), box_debug_extents, rotation);
			idBox end_box(currentGoal.origin, box_debug_extents, rotation);

			gameRenderWorld->DebugBox(color_red, start_box);
			gameRenderWorld->DebugBox(color_red, end_box);
		}
	}
}


/*
=======================
rvmBot::PresenceTypeBoundingBox
=======================
*/
void rvmBot::PresenceTypeBoundingBox(int presencetype, idVec3 &mins, idVec3 &maxs)
{
	int index;
	//bounding box size for each presence type
	//idVec3 boxmins[3] = { {0, 0, 0}, {-15, -15, -24}, {-15, -15, -24} };
	//idVec3 boxmaxs[3] = { {0, 0, 0}, { 15,  15,  32}, { 15,  15,   8} };

	idVec3 boxmins[3];
	idVec3 boxmaxs[3];

	boxmins[0] = idVec3(0, 0, 0);
	boxmins[1] = idVec3(-15, -15, -24);
	boxmins[2] = idVec3(-15, -15, -24);

	boxmaxs[0] = idVec3(0, 0, 0);
	boxmaxs[1] = idVec3(15, 15, 32);
	boxmaxs[2] = idVec3(15, 15, 8);


	if (presencetype == PRESENCE_NORMAL)
		index = 1;
	else if (presencetype == PRESENCE_CROUCH)
		index = 2;
	else
	{
		//botimport.Print(PRT_FATAL, "AAS_PresenceTypeBoundingBox: unknown presence type\n");
		index = 2;
	}
	mins = boxmins[index];
	maxs = boxmaxs[index];
}


/*
===================
rvmBot::Think
===================
*/
void rvmBot::Think(void) {
	if(gameLocal.isServer)
	{
		ServerThink();
	}

	deltaViewAngles.Zero();

	idPlayer::Think();
}