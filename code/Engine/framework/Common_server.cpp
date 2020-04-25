// Common_server.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "Common_local.h"
#include "Common_network.h"
#include "Session_local.h"

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
	if (serverState != NULL) {
		common->FatalError("Server already running!\n");
		return;
	}

	serverState = new rvmNetworkServerState_t();
	serverState->address.host = ENET_HOST_ANY;
	serverState->address.port = PORT_SERVER;

	networkType = NETWORK_TYPE_SERVER;

	networkClients.SetNum(net_maxPlayers.GetInteger());
	for (int i = 0; i < networkClients.Num(); i++) {
		networkClients[i] = NULL;
	}

	// Create the enet server.
	common->Printf("Starting Server on port %d\n", PORT_SERVER);
	serverState->server = enet_host_create(&serverState->address, net_maxPlayers.GetInteger(), 2, 0, 0);
	if (serverState->server == NULL) {
		common->FatalError("Failed to start server!\n");
	}

	// Stop any current game.
	session->Stop();
	sessLocal.ExecuteServerMapChange();

	if (userCmds != NULL) {
		common->FatalError("userCmds not freed in network!\n");
	}

	userCmds = new usercmd_t[net_maxPlayers.GetInteger()];
	memset(userCmds, 0, sizeof(usercmd_t) * net_maxPlayers.GetInteger());

	if (!IsDedicatedServer()) {
		localClientNum = 0;
		game->SetLocalClient(localClientNum);

		// Allocate a new client and begin the connection process.
		NewClient(0, NULL);
		game->ServerNewClient(localClientNum);
	}
	else {
		localClientNum = -1;
	}
}

/*
====================
idCommonLocal::BindUserInfo
====================
*/
void idCommonLocal::BindUserInfo(int clientNum) {
	game->SetUserInfo(clientNum, sessLocal.mapSpawnData.userInfo[clientNum], false);
}

/*
====================
idCommonLocal::InitNetwork
====================
*/
void idCommonLocal::SetUserInfoKey(int clientNum, const char* key, const char* value) {
	sessLocal.mapSpawnData.userInfo[clientNum].Set(key, value);
}


/*
==================
idCommonLocal::AllocateClientSlotForBot
==================
*/
int idCommonLocal::AllocateClientSlotForBot(int maxPlayersOnServer) {
	for (int i = 0; i < networkClients.Num(); i++) {
		if (networkClients[i] == NULL || networkClients[i]->addrType == NA_BAD) {
			NewClient(i, NULL);

			idStr botName;
			game->GetRandomBotName(i, botName);
			sessLocal.mapSpawnData.userInfo[i].Set("ui_name", botName);

			game->SetUserInfo(i, sessLocal.mapSpawnData.userInfo[i], false);
			game->ServerBotBegin(i, botName);
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
	if (networkClients[clientNum]->addrType != NA_BOT) {
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
	if (!IsServer()) {
		common->FatalError("You can't send reliable messages to other clients if you aren't the server!\n");
		return;
	}

	if (clientNum == -1) {
		for (int i = 0; i < net_maxPlayers.GetInteger(); i++)
		{
			if (networkClients[i] == NULL)
				continue;

			if (networkClients[i]->addrType == NA_BOT) {
				continue;
			}

			if (networkClients[i]->addrType == NA_LOOPBACK) {
				rvmNetworkPacketQueue_t* newPacket = new rvmNetworkPacketQueue_t(msg);
				newPacket->clientNum = i;
				clientPacketQueue.AddToEnd(newPacket);
				continue;
			}

			if (networkClients[i]->peer == NULL) {
				FatalError("ServerSendReliableMessage peer is NULL\n");
				continue;
			}

			ENetPacket* packet = enet_packet_create(msg.GetData(), msg.GetSize(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(networkClients[i]->peer, 0, packet);
		}

		return;
	}

	if (networkClients[clientNum]->addrType == NA_LOOPBACK) {
		rvmNetworkPacketQueue_t* newPacket = new rvmNetworkPacketQueue_t(msg);
		newPacket->clientNum = clientNum;
		clientPacketQueue.AddToEnd(newPacket);
		return;
	}

	if (networkClients[clientNum]->addrType == NA_BOT) {
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
idCommonLocal::GetLocalClientNum
==================
*/
void idCommonLocal::NewClient(int clientNum, void* peer) {
	networkClients[clientNum] = new rvmNetworkClient_t();

	if (peer == NULL) {
		networkClients[clientNum]->addrType = NA_LOOPBACK;
	}
	else {
		networkClients[clientNum]->peer = (ENetPeer*)peer;
		networkClients[clientNum]->addrType = NA_IP;
	}

	// let the game know a player connected
	game->ServerClientConnect(clientNum, "");
}

