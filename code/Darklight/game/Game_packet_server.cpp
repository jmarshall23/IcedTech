// Game_packet_server.cpp
//

#include "Game_precompiled.h"

/*
====================
idGameLocal::ServerProcessPacket
====================
*/
void idGameLocal::ServerProcessPacket(int clientNum, const idBitMsg& msg) {
	int opCode = msg.ReadUShort();
	switch(opCode) {
		case NET_OPCODE_SENDUSERINFO:
			{
				char playerName[256];
				msg.ReadString(playerName, sizeof(playerName));
				common->SetUserInfoKey(clientNum, "ui_name", playerName);
				common->BindUserInfo(clientNum);

				// We have just recieved the user info. 
				// TODO: We need to verify against a ban list or something, for now we are just going to accept the connection.
				rvmNetworkPacket packet(NETWORK_PACKET_WRITE);
				
				idStr mapName = cvarSystem->GetCVarString("si_map");
				idStr gameType = cvarSystem->GetCVarString("si_gametype");
				packet.msg.WriteUShort(NET_OPCODE_MAPCHANGE);
				packet.msg.WriteString(mapName);
				packet.msg.WriteString(gameType);
				common->ServerSendReliableMessage(clientNum, packet.msg);
			}
			break;
		case NET_OPCODE_CLIENTLOADED:
			{
				ServerClientBegin(localClientNum, false, "");
			}
			break;
		default:
			common->Warning("ServerProcessPacket: Unknown OpCode %d\n", opCode);
			break;
	}
}

/*
====================
idGameLocal::ServerProcessPacket
====================
*/
void idGameLocal::ServerNewClient(int clientNum) {
	// When a client connects we need to get there userinfo.
	rvmNetworkPacket newClientPacket(NETWORK_PACKET_WRITE);
	newClientPacket.msg.WriteUShort(NET_OPCODE_GETUSERINFO);
	common->ServerSendReliableMessage(clientNum, newClientPacket.msg);
}

/*
================
idGameLocal::ServerClientBegin
================
*/
void idGameLocal::ServerClientBegin(int clientNum, bool isBot, const char* botName) {
	// initialize the decl remap
	InitClientDeclRemap(clientNum);

	// send message to initialize decl remap at the client (this is always the very first reliable game message)
	rvmNetworkPacket declRemapPacket(NETWORK_PACKET_WRITE);
	declRemapPacket.msg.WriteUShort(NET_OPCODE_INIT_DECL_REMAP);
	common->ServerSendReliableMessage(clientNum, declRemapPacket.msg);

	// spawn the player
	SpawnPlayer(clientNum, isBot, botName);
	if (clientNum == localClientNum) {
		mpGame.EnterGame(clientNum);
	}

	// send message to spawn the player at the clients
	rvmNetworkPacket spawnPlayerPacket(NETWORK_PACKET_WRITE);
	spawnPlayerPacket.msg.WriteUShort(NET_OPCODE_SPAWNPLAYER);
	spawnPlayerPacket.msg.WriteByte(clientNum);
	spawnPlayerPacket.msg.WriteLong(spawnIds[clientNum]);
	common->ServerSendReliableMessage(-1, spawnPlayerPacket.msg);
}

/*
================
idGameLocal::ServerSendChatMessage
================
*/
void idGameLocal::ServerSendChatMessage(int to, const char* name, const char* text) {
	rvmNetworkPacket messagePacket(NETWORK_PACKET_WRITE);
	messagePacket.msg.WriteUShort(NET_OPCODE_CHATMESSAGE);
	messagePacket.msg.WriteString(name);
	messagePacket.msg.WriteString(text, -1, false);;
	common->ServerSendReliableMessage(to, messagePacket.msg);

	if (to == -1 || to == localClientNum) {
		mpGame.AddChatLine("%s^0: %s\n", name, text);
	}
}