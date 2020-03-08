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
	bs.action = NULL;
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
==================
rvmBot::BotUpdateInventory
==================
*/
void rvmBot::BotUpdateInventory(void) {
	//bs.inventory[INVENTORY_ARMOR]			= bs->cur_ps.stats[STAT_ARMOR];
	//bs.inventory[INVENTORY_GAUNTLET]		= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_GAUNTLET)) != 0;
	//bs.inventory[INVENTORY_SHOTGUN]			= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_SHOTGUN)) != 0;
	//bs.inventory[INVENTORY_MACHINEGUN]		= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_MACHINEGUN)) != 0;
	//bs.inventory[INVENTORY_GRENADELAUNCHER] = (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_GRENADE_LAUNCHER)) != 0;
	//bs.inventory[INVENTORY_ROCKETLAUNCHER]	= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_ROCKET_LAUNCHER)) != 0;
	//bs.inventory[INVENTORY_LIGHTNING]		= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_LIGHTNING)) != 0;
	//bs.inventory[INVENTORY_RAILGUN]			= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_RAILGUN)) != 0;
	//bs.inventory[INVENTORY_PLASMAGUN]		= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_PLASMAGUN)) != 0;
	//bs.inventory[INVENTORY_BFG10K]			= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_BFG)) != 0;
	//bs.inventory[INVENTORY_GRAPPLINGHOOK]	= (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_GRAPPLING_HOOK)) != 0;
	//bs.inventory[INVENTORY_SHELLS]			= bs->cur_ps.ammo[WP_SHOTGUN];
	//bs.inventory[INVENTORY_BULLETS]			= bs->cur_ps.ammo[WP_MACHINEGUN];
	//bs.inventory[INVENTORY_GRENADES]		= bs->cur_ps.ammo[WP_GRENADE_LAUNCHER];
	//bs.inventory[INVENTORY_CELLS] = bs->cur_ps.ammo[WP_PLASMAGUN];
	//bs.inventory[INVENTORY_LIGHTNINGAMMO] = bs->cur_ps.ammo[WP_LIGHTNING];
	//bs.inventory[INVENTORY_ROCKETS] = bs->cur_ps.ammo[WP_ROCKET_LAUNCHER];
	//bs.inventory[INVENTORY_SLUGS] = bs->cur_ps.ammo[WP_RAILGUN];
	//bs.inventory[INVENTORY_BFGAMMO] = bs->cur_ps.ammo[WP_BFG];
	//bs.inventory[INVENTORY_HEALTH] = bs->cur_ps.stats[STAT_HEALTH];
	//bs.inventory[INVENTORY_TELEPORTER] = bs->cur_ps.stats[STAT_HOLDABLE_ITEM] == MODELINDEX_TELEPORTER;
	//bs.inventory[INVENTORY_MEDKIT] = bs->cur_ps.stats[STAT_HOLDABLE_ITEM] == MODELINDEX_MEDKIT;
	//bs.inventory[INVENTORY_QUAD] = bs->cur_ps.powerups[PW_QUAD] != 0;
	//bs.inventory[INVENTORY_ENVIRONMENTSUIT] = bs->cur_ps.powerups[PW_BATTLESUIT] != 0;
	//bs.inventory[INVENTORY_HASTE] = bs->cur_ps.powerups[PW_HASTE] != 0;
	//bs.inventory[INVENTORY_INVISIBILITY] = bs->cur_ps.powerups[PW_INVIS] != 0;
	//bs.inventory[INVENTORY_REGEN] = bs->cur_ps.powerups[PW_REGEN] != 0;
	//bs.inventory[INVENTORY_FLIGHT] = bs->cur_ps.powerups[PW_FLIGHT] != 0;
	//bs.inventory[INVENTORY_REDFLAG] = bs->cur_ps.powerups[PW_REDFLAG] != 0;
	//bs.inventory[INVENTORY_BLUEFLAG] = bs->cur_ps.powerups[PW_BLUEFLAG] != 0;
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
	bs.botinput.dir = bs.currentGoal.nextMoveOrigin - GetPhysics()->GetOrigin();

	idAngles ang(0, bs.botinput.dir.ToYaw(), 0);
	bs.botinput.viewangles = ang;
	bs.botinput.speed = 200;

	bs.botinput.dir.Normalize();
}

/*
===================
rvmBot::SpawnToPoint
===================
*/
void rvmBot::SpawnToPoint(const idVec3& spawn_origin, const idAngles& spawn_angles) {
	idPlayer::SpawnToPoint(spawn_origin, spawn_angles);

	if (gameLocal.isServer) {
		bs.ltg_time = 0;
		bs.action = NULL;
	}
}

/*
===================
rvmBot::ServerThink
===================
*/
void rvmBot::ServerThink(void) {
	bs.origin = GetPhysics()->GetOrigin();
	bs.eye = GetEyePosition();
	bs.viewangles = viewAngles;

	BotUpdateInventory();

	if (bot_pathdebug.IsModified())
	{
		if (bot_pathdebug.GetBool())
		{
			bs.currentGoal.origin = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin();
			bs.currentGoal.framenum = gameLocal.framenum;
		}

		bot_pathdebug.ClearModified();
		bot_pathdebug.SetBool(false);
	}

	if (bs.action == NULL) {
		bs.action = &botAIActionSeekLTG;
	}

	bs.action->Think(&bs);

	// If we have a new goal position, let's figure out how to get there.
	if (bs.currentGoal.framenum == gameLocal.framenum) {
		if (gameLocal.NavGetPathBetweenPoints(GetPhysics()->GetOrigin(), bs.currentGoal.origin, navWaypoints))
		{
			currentWaypoint = 0;
		}
		else
		{
			currentWaypoint = BOT_NOWAYPOINT;
		}
	}

	// If we are moving along a set of waypoints, let's move along.
	if (currentWaypoint != BOT_NOWAYPOINT)
	{
		if (currentWaypoint < navWaypoints.Num())
		{
			if (botGoalManager.BotNearGoal(GetPhysics()->GetOrigin(), navWaypoints[currentWaypoint]))
			{
				currentWaypoint++;
				if (currentWaypoint >= navWaypoints.Num())
					currentWaypoint = navWaypoints.Num() - 1;
			}
		}

		bs.currentGoal.nextMoveOrigin = navWaypoints[currentWaypoint];

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
			idBox end_box(bs.currentGoal.origin, box_debug_extents, rotation);

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