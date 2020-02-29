// Bot.cpp
//

#include "game_precompiled.h"
#include "../Game_local.h"

idCVar bot_pathdebug("bot_pathdebug", "0", CVAR_BOOL | CVAR_CHEAT, "force the bot to path to player");

CLASS_DECLARATION(idPlayer, rvmBot)
END_CLASS

/*
===================
rvmBot::rvmBot
===================
*/
rvmBot::rvmBot() {
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
	idPlayer::Spawn();
}

/*
===================
rvmBot::Think
===================
*/
void rvmBot::Think(void) {
	if(bot_pathdebug.IsModified())
	{
		if(bot_pathdebug.GetBool())
		{
			
		}

		bot_pathdebug.ClearModified();
		bot_pathdebug.SetBool(false);
	}

	idPlayer::Think();
}