// Common_network.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "Common_local.h"
#include "Common_network.h"
#include "Session_local.h"

idCVar net_maxPlayers("net_maxPlayers", "32", CVAR_INTEGER | CVAR_SYSTEM, "max players in a game");

/*
==================
idCommonLocal::SpawnServer_f
==================
*/
void idCommonLocal::SpawnServer_f(const idCmdArgs& args) {
	if (args.Argc() > 1) {
		cvarSystem->SetCVarString("si_map", args.Argv(1));
	}

	// don't let a server spawn with singleplayer game type - it will crash
	if (idStr::Icmp(cvarSystem->GetCVarString("si_gameType"), "singleplayer") == 0) {
		cvarSystem->SetCVarString("si_gameType", "deathmatch");
	}
	com_asyncInput.SetBool(false);
	// make sure the current system state is compatible with net_serverDedicated
	switch (cvarSystem->GetCVarInteger("net_serverDedicated")) {
	case 0:
	case 2:
		if (!renderSystem->IsOpenGLRunning()) {
			common->Warning("OpenGL is not running, net_serverDedicated == %d", cvarSystem->GetCVarInteger("net_serverDedicated"));
		}
		break;
	case 1:
		if (renderSystem->IsOpenGLRunning()) {
			Sys_ShowConsole(1, false);
			renderSystem->ShutdownOpenGL();
		}
		soundSystem->SetMute(true);
		soundSystem->Shutdown();
		break;
	}

	commonLocal.SpawnServer();
}

/*
====================
idCommonLocal::SpawnServer
====================
*/
void idCommonLocal::SpawnServer(void) {
	if(serverState != NULL) {
		common->FatalError("Server already running!\n");
		return;
	}

	serverState = new rvmNetworkServerState_t();
	serverState->address.host = ENET_HOST_ANY;
	serverState->address.port = PORT_SERVER;

	networkType = NETWORK_TYPE_SERVER;

	networkClients.SetNum(net_maxPlayers.GetInteger());
	for(int i = 0; i < networkClients.Num(); i++) {
		networkClients[i] = NULL;
	}

	// Create the enet server.
	common->Printf("Starting Server on port %d\n", PORT_SERVER);
	serverState->server = enet_host_create(&serverState->address, net_maxPlayers.GetInteger(), 2, 0, 0);
	if(serverState->server == NULL) {
		common->FatalError("Failed to start server!\n");
	}

	// Stop any current game.
	session->Stop();
	sessLocal.ExecuteServerMapChange();	

	if(userCmds != NULL) {
		common->FatalError("userCmds not freed in network!\n");
	}

	userCmds = new usercmd_t[net_maxPlayers.GetInteger()];
	memset(userCmds, 0, sizeof(usercmd_t) * net_maxPlayers.GetInteger());

	if(!IsDedicatedServer()) {
		localClientNum = 0;
		game->SetLocalClient(localClientNum);

		NewClient(0, NULL);		
		game->SetUserInfo(localClientNum, sessLocal.mapSpawnData.userInfo[localClientNum], false);
		game->ServerClientBegin(localClientNum, false, "");		
	}
	else {
		localClientNum = -1;
	}
}

/*
====================
idCommonLocal::InitNetwork
====================
*/
void idCommonLocal::InitNetwork(void) {
	userCmds = NULL;

	// Init the enet library.
	common->Printf("Init enet library...\n");
	if (enet_initialize() != 0) {
		common->FatalError("Failed to init enet library!\n");
	}

	// Init the multiplayer commands.
	cmdSystem->AddCommand("spawnServer", SpawnServer_f, CMD_FL_SYSTEM, "spawns a server", idCmdSystem::ArgCompletion_MapName);
}

/*
====================
idCommonLocal::ShutdownNetwork
====================
*/
void idCommonLocal::ShutdownNetwork(void) {
	if(serverState) {
		if (serverState->server) {
			enet_host_destroy(serverState->server);
		}

		delete serverState;
		serverState = NULL;
	}

	for(int i = 0; i < networkClients.Num(); i++) {
		if (networkClients[i] == NULL)
			continue;

		if (networkClients[i]->client != NULL) {
			enet_host_destroy(networkClients[i]->client);
		}
		delete networkClients[i];
	}

	if(userCmds) {
		delete userCmds;
		userCmds = NULL;
	}

	networkClients.SetNum(0);
	networkType = NETWORK_TYPE_NONE;
	localClientNum = 0;
}

/*
==================
idCommonLocal::AllocateClientSlotForBot
==================
*/
int idCommonLocal::AllocateClientSlotForBot(int maxPlayersOnServer) {
	for(int i = 0; i < networkClients.Num(); i++) {
		if(networkClients[i] == NULL || networkClients[i]->addrType == NA_BAD) {
			NewClient(i, NULL);

			idStr botName;
			game->GetRandomBotName(i, botName);
			sessLocal.mapSpawnData.userInfo[i].Set("ui_name", botName);

			game->SetUserInfo(i, sessLocal.mapSpawnData.userInfo[i], false);
			game->ServerClientBegin(i, true, botName);
			networkClients[i]->addrType = NA_BOT;			
			return i;
		}
	}

	Warning("Not enough player slots for bot!\n");
	return -1;
}

/*
==================
idCommonLocal::ServerSetBotUserCommand
==================
*/
int	idCommonLocal::ServerSetBotUserCommand(int clientNum, int frameNum, const usercmd_t& cmd) {
	if(networkClients[clientNum]->addrType != NA_BOT) {
		common->FatalError("ServerSetBotUserCommand: clientNum isn't a bot!\n");
		return 0;
	}

	memcpy(&userCmds[clientNum], &cmd, sizeof(cmd));

	return 0;
}

int	idCommonLocal::ServerSetBotUserName(int clientNum, const char* playerName) {
	return 0;
}

/*
==================
idCommonLocal::ServerSendReliableMessage
==================
*/
void idCommonLocal::ServerSendReliableMessage(int clientNum, const idBitMsg& msg) {
	if(!IsServer()) {
		common->FatalError("You can't send reliable messages to other clients if you aren't the server!\n");
		return;
	}

	if (clientNum == -1) {
		for (int i = 0; i < net_maxPlayers.GetInteger(); i++)
		{
			if (networkClients[i] == NULL)
				continue;

			if(networkClients[i]->addrType == NA_BOT) {				
				continue;
			}

			if (networkClients[i]->addrType == NA_LOOPBACK) {
				idBitMsg clientMsg;
				clientMsg.Init(msg.GetData(), msg.GetSize());
				game->ClientProcessPacket(i, clientMsg);
				continue;
			}

			if(networkClients[i]->peer == NULL) {
				FatalError("ServerSendReliableMessage peer is NULL\n");
				continue;
			}

			ENetPacket* packet = enet_packet_create(msg.GetData(), msg.GetSize(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(networkClients[i]->peer, 0, packet);
		}

		return;
	}

	if (networkClients[clientNum]->addrType == NA_BOT || networkClients[clientNum]->addrType == NA_LOOPBACK) {
		return;
	}

	if (networkClients[clientNum]->addrType != NA_IP) {
		common->FatalError("Tried to send a reliable message to a disconnected client!\n");
		return;
	}

	ENetPacket* packet = enet_packet_create(msg.GetData(), msg.GetSize(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(networkClients[clientNum]->peer, 0, packet);
}

/*
==================
idCommonLocal::ServerSendReliableMessageExcluding
==================
*/
void idCommonLocal::ServerSendReliableMessageExcluding(int clientNum, const idBitMsg& msg) {
	for (int i = 0; i < networkClients.Num(); i++) {
		if (networkClients[i]->addrType == NA_IP && i != clientNum) {
			ServerSendReliableMessage(i, msg);
		}
	}
}

/*
==================
idCommonLocal::ClientSendReliableMessage
==================
*/
void idCommonLocal::ClientSendReliableMessage(const idBitMsg& msg) {
	if(!IsDedicatedServer() && IsServer() && localClientNum != -1) {
		idBitMsg serverMsg;
		serverMsg.Init(msg.GetData(), msg.GetSize());
		game->ServerProcessPacket(localClientNum, serverMsg);
		return;
	}
}

int idCommonLocal::ClientGetTimeSinceLastPacket(void) {
	return 0;
}

/*
==================
idCommonLocal::IsClient
==================
*/
bool idCommonLocal::IsClient(void) {
	return networkType == NETWORK_TYPE_CLIENT;
}

/*
==================
idCommonLocal::IsServer
==================
*/
bool idCommonLocal::IsServer(void) {
	return networkType == NETWORK_TYPE_SERVER;
}

/*
==================
idCommonLocal::IsMultiplayer
==================
*/
bool idCommonLocal::IsMultiplayer(void) {
	return networkType != NETWORK_TYPE_NONE;
}

/*
==================
idCommonLocal::DisconnectFromServer
==================
*/
void idCommonLocal::DisconnectFromServer(void) {

}

/*
==================
idCommonLocal::KillServer
==================
*/
void idCommonLocal::KillServer(void) {

}

/*
==================
idCommonLocal::ServerGetClientTimeSinceLastPacket
==================
*/
int idCommonLocal::ServerGetClientTimeSinceLastPacket(int clientNum) {
	return 0;
}

/*
==================
idCommonLocal::ServerGetClientPing
==================
*/
int idCommonLocal::ServerGetClientPing(int clientNum) {
	return 10;
}

/*
==================
idCommonLocal::RunNetworkThink
==================
*/
void idCommonLocal::RunNetworkThink(int numGameFrames) {
	if(!IsMultiplayer()) {
		return;
	}

	if (IsServer()) {
		ENetEvent ev;
		enet_host_service(serverState->server, &ev, 0);
	}

	if (!IsDedicatedServer() && IsServer() && localClientNum != -1) {
		usercmd_t cmd = usercmdGen->GetDirectUsercmd();
		memcpy(&userCmds[0], &cmd, sizeof(usercmd_t));
	}

	// advance game
	for (int i = 0; i < numGameFrames; i++) {
		gameReturn_t ret = game->RunFrame(&userCmds[0]);
	}
}

/*
==================
idCommonLocal::GetLocalClientNum
==================
*/
int idCommonLocal::GetLocalClientNum(void) {
	return localClientNum;
}
/*
==================
idCommonLocal::GetLocalClientNum
==================
*/

void idCommonLocal::NewClient(int clientNum, void* peer) {
	networkClients[clientNum] = new rvmNetworkClient_t();
	
	if (peer == NULL) {
		networkClients[clientNum]->addrType = NA_LOOPBACK;
	}
	else {
		networkClients[clientNum]->peer = (ENetPeer *)peer;
		networkClients[clientNum]->addrType = NA_IP;
	}

	// let the game know a player connected
	game->ServerClientConnect(clientNum, "");
}