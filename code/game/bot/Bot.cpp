// Bot.cpp
//

#include "game_precompiled.h"
#include "../Game_local.h"

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
	idPlayer::Think();
}