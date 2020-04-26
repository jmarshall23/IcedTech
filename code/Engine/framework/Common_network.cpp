// Common_network.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "Common_local.h"
#include "Common_network.h"
#include "Session_local.h"

idCVar net_maxPlayers("net_maxPlayers", "32", CVAR_INTEGER | CVAR_SYSTEM, "max players in a game");

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
	cmdSystem->AddCommand("connect", Connect_f, CMD_FL_SYSTEM, "connects to a server");
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
idCommonLocal::NetworkFrame
==================
*/
void idCommonLocal::NetworkFrame(int numGameFrames) {
	if(!IsMultiplayer()) {
		return;
	}

	// if the console is down, we don't need to hold
	// the mouse cursor
	if (console->Active() || common->IsEditorRunning()) {
		Sys_GrabMouseCursor(false);
	}
	else {
		Sys_GrabMouseCursor(true);
	}

	ENetEvent ev;
	ENetHost* host = NULL;

	if(IsServer()) {
		host = serverState->server;
	}
	else {
		host = networkServer->client;
	}

	while (enet_host_service(host, &ev, 0) > 0) {
		if (IsServer()) {
			if (ev.type == ENET_EVENT_TYPE_CONNECT) {
				common->Printf("Incoming client detected...\n");

				for (int i = 0; i < net_maxPlayers.GetInteger(); i++) {
					if (networkClients[i] != NULL)
						continue;

					common->Printf("...new client initialized on id %d\n", i);

					networkClients[i] = new rvmNetworkClient_t();
					networkClients[i]->peer = ev.peer;
					networkClients[i]->address = ev.peer->address;
					networkClients[i]->addrType = NA_IP;
					networkClients[i]->client = NULL;

					NewClient(i, networkClients[i]->peer);
					game->ServerNewClient(i);
					break;
				}

				continue;
			}
		}

		if(ev.type == ENET_EVENT_TYPE_RECEIVE) {
			idBitMsg msg;

			msg.Init((const byte*)ev.packet->data, ev.packet->dataLength);
			msg.SetSize(ev.packet->dataLength);
			msg.BeginReading();
			rvmNetworkPacketQueue_t* packet = new rvmNetworkPacketQueue_t(msg);
			packet->clientNum = packet->_msg.ReadUShort();

			if (IsServer()) {
				serverPacketQueue.AddToEnd(packet);
			}
			else if(IsClient()) {
				// This is a hack!
				if (localClientNum == -1) {
					localClientNum = packet->clientNum;
					game->SetLocalClient(localClientNum);
				}
				clientPacketQueue.AddToEnd(packet);
			}
			
			enet_packet_destroy(ev.packet);
			continue;
		}

		if(ev.type == ENET_EVENT_TYPE_DISCONNECT) {
			if(IsClient()) {
				DisconnectFromServer();
				return;
			}
		}
	}

	// Run the server commands.
	if (IsDedicatedServer() || IsServer())
	{
		rvmNetworkPacketQueue_t* serverPacket = serverPacketQueue.GetFirst();
		while (true) {
			if (serverPacket == NULL)
				break;

			game->ServerProcessPacket(serverPacket->clientNum, serverPacket->_msg);

			rvmNetworkPacketQueue_t* oldPacket = serverPacket;
			serverPacket = serverPacket->listNode.GetNext();
			serverPacketQueue.Remove(oldPacket);
		}
	}

	// If we aren't a dedicated server, we will have client commands.
	if (!IsDedicatedServer()) {
		rvmNetworkPacketQueue_t* clientPacket = clientPacketQueue.GetFirst();
		while (true) {
			if (clientPacket == NULL)
				break;

			game->ClientProcessPacket(localClientNum, clientPacket->_msg);

			rvmNetworkPacketQueue_t* oldPacket = clientPacket;
			clientPacket = clientPacket->listNode.GetNext();
			clientPacketQueue.Remove(oldPacket);
		}
	}
		

	if (!IsDedicatedServer() && localClientNum != -1) {
		usercmd_t cmd = usercmdGen->GetDirectUsercmd();
		memcpy(&userCmds[localClientNum], &cmd, sizeof(usercmd_t));
	}

	// advance game
	for (int i = 0; i < numGameFrames; i++) {
		if (IsServer()) {
			gameReturn_t ret = game->RunFrame(&userCmds[0]);
		}
		else if(userCmds != NULL) {
			gameReturn_t ret = game->ClientPrediction(localClientNum, &userCmds[0], false);
		}
	}

	enet_host_flush(host);
}

/*
==================
idCommonLocal::GetLocalClientNum
==================
*/
int idCommonLocal::GetLocalClientNum(void) {
	return localClientNum;
}
