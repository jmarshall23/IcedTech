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
				rvmNetworkPacket packet(clientNum);
				
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
				ServerClientBegin(clientNum, false, "");
			}
			break;
		case NET_OPCODE_IMPULSE:
			{
				int _spawnId = msg.ReadBits(32);
				int _gameTime = msg.ReadLong();
				int _impulse = msg.ReadBits(6);

				if (_impulse > 0) {
					entities[clientNum]->Cast<idPlayer>()->PerformImpulse(_impulse);
				}
			}
			break;
		default:
			common->Warning("ServerProcessPacket: Unknown OpCode %d\n", opCode);
			break;
	}
}

/*
====================
idGameLocal::ServerToggleSpectate
====================
*/
void idGameLocal::ServerToggleSpectate(int clientNum, bool spectate) {
	entities[clientNum]->Cast<idPlayer>()->Spectate(spectate);

	rvmNetworkPacket spectatePacket(clientNum);
	spectatePacket.msg.WriteUShort(NET_OPCODE_SPECTATE);
	spectatePacket.msg.WriteByte(clientNum);
	spectatePacket.msg.WriteByte(spectate);
	common->ServerSendReliableMessage(clientNum, spectatePacket.msg);
}

/*
====================
idGameLocal::ServerProcessPacket
====================
*/
void idGameLocal::ServerNewClient(int clientNum) {
	// When a client connects we need to get there userinfo.
	rvmNetworkPacket newClientPacket(clientNum);
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
	rvmNetworkPacket declRemapPacket(clientNum);
	declRemapPacket.msg.WriteUShort(NET_OPCODE_INIT_DECL_REMAP);
	common->ServerSendReliableMessage(clientNum, declRemapPacket.msg);

	// spawn the player
	SpawnPlayer(clientNum, isBot, botName);
	if (clientNum == localClientNum) {
		mpGame.EnterGame(clientNum);
	}

	// send message to spawn the player at the clients
	rvmNetworkPacket spawnPlayerPacket(clientNum);
	spawnPlayerPacket.msg.WriteUShort(NET_OPCODE_SPAWNPLAYER);
	spawnPlayerPacket.msg.WriteByte(clientNum);
	spawnPlayerPacket.msg.WriteLong(spawnIds[clientNum]);
	common->ServerSendReliableMessage(-1, spawnPlayerPacket.msg);

	// Disable spectate on the client.
	ServerToggleSpectate(clientNum, false);
}

/*
================
idGameLocal::ServerSendChatMessage
================
*/
void idGameLocal::ServerSendChatMessage(int to, const char* name, const char* text) {
	rvmNetworkPacket messagePacket(to);
	messagePacket.msg.WriteUShort(NET_OPCODE_CHATMESSAGE);
	messagePacket.msg.WriteString(name);
	messagePacket.msg.WriteString(text, -1, false);;
	common->ServerSendReliableMessage(to, messagePacket.msg);

	if (to == -1 || to == localClientNum) {
		mpGame.AddChatLine("%s^0: %s\n", name, text);
	}
}

/*
================
idGameLocal::ServerSendChatMessage
================
*/
void idGameLocal::WriteNetworkSnapshots(void) {
	for (int i = 0; i < MAX_ASYNC_CLIENTS; i++) {
		if (entities[i] == NULL)
			continue;

		rvmNetworkPacket snapshotPacket(i, 1024);
		snapshotPacket.msg.WriteUShort(NET_OPCODE_SNAPSHOT);
		ServerWriteSnapshot(i, 0, snapshotPacket.msg, NULL, snapShotSequence);		
		common->ServerSendReliableMessage(i, snapshotPacket.msg);
	}

	snapShotSequence++;
}