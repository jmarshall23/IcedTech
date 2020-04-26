// Common_client.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "Common_local.h"
#include "Common_network.h"
#include "Session_local.h"

/*
==================
idCommonLocal::Connect_f
==================
*/
void idCommonLocal::Connect_f(const idCmdArgs& args) {
	if (common->IsServer()) {
		common->Printf("already running a server\n");
		return;
	}
	if (args.Argc() != 2) {
		common->Printf("USAGE: connect <serverName>\n");
		return;
	}
	com_asyncInput.SetBool(false);
	commonLocal.ConnectToServer(args.Argv(1));
}

/*
==================
idCommonLocal::ConnectToServer
==================
*/
void idCommonLocal::ConnectToServer(const char* ip) {
	ENetAddress address;
	ENetEvent event;

	enet_address_set_host(&address, ip);
	address.port = PORT_SERVER;

	// Create our network server interface.
	networkServer = new rvmNetworkServer_t();
	networkServer->client = enet_host_create(NULL, 1, 2, 0, 0);
	if(networkServer->client == NULL) {
		common->FatalError("Failed to create enet client!\n");
		return;
	}

	// Connect to the server.
	common->Printf("Connecting to server %s:%d\n", ip, address.port);
	networkServer->serverPeer = enet_host_connect(networkServer->client, &address, 2, 0);
	if(networkServer->serverPeer == NULL) {
		common->FatalError("No available peers for init an enet connection!\n");
		return;
	}

	// Wait for 5 seconds for a connection.
	if (enet_host_service(networkServer->client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		common->Printf("Connected to server!\n");		
	}
	else {
		common->Warning("Failed to connect to server!\n");
		enet_peer_reset(networkServer->serverPeer);
		enet_host_destroy(networkServer->client);
		delete networkServer;
		return;
	}

	networkType = NETWORK_TYPE_CLIENT;
	localClientNum = -1;

	userCmds = new usercmd_t[net_maxPlayers.GetInteger()];
	memset(userCmds, 0, sizeof(usercmd_t) * net_maxPlayers.GetInteger());
}

/*
==================
idCommonLocal::ClientSendReliableMessage
==================
*/
void idCommonLocal::ClientSendReliableMessage(const idBitMsg& msg) {
	if (!IsDedicatedServer() && IsServer() && localClientNum != -1) {
		rvmNetworkPacketQueue_t* newPacket = new rvmNetworkPacketQueue_t(msg);
		newPacket->clientNum = localClientNum;
		serverPacketQueue.AddToEnd(newPacket);
		newPacket->_msg.ReadUShort(); // Client Number
		return;
	}

	ENetPacket* packet = enet_packet_create(msg.GetData(), msg.GetSize(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(networkServer->serverPeer, 0, packet);
}

int idCommonLocal::ClientGetTimeSinceLastPacket(void) {
	return 0;
}

/*
==================
idCommonLocal::ExecuteClientMapChange
==================
*/
void idCommonLocal::ExecuteClientMapChange(const char* mapName, const char* gameType) {
	cvarSystem->SetCVarString("si_map", mapName);
	cvarSystem->SetCVarString("si_gameType", "deathmatch");

	sessLocal.ExecuteServerMapChange();
}


/*
==================
idCommonLocal::DisconnectFromServer
==================
*/
void idCommonLocal::DisconnectFromServer(void) {
	if (networkServer == NULL)
		return;

	common->Printf("Disconnected from the server.\n");

	enet_peer_reset(networkServer->serverPeer);
	enet_host_destroy(networkServer->client);

	delete networkServer;
	networkServer = NULL;
	
	networkType = NETWORK_TYPE_NONE;

	// Server disconnected, drop out and return to the main menu(todo take us to a score screen of some kind?).
	sessLocal.Stop();
}
