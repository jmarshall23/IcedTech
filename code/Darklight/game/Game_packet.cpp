// Game_packet.cpp
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
		case NET_OPCODE_UPDATEPLAYER:
			{
				if (entities[clientNum] != NULL) {
					idPlayer* player = entities[clientNum]->Cast<idPlayer>();
					usercmd_t& cmd = gameLocal.usercmds[clientNum];

					cmd.gameTime = gameLocal.time;
					cmd.gameFrame = gameLocal.framenum;
					cmd.duplicateCount = 0;

					cmd.forwardmove = msg.ReadByte();
					cmd.rightmove = msg.ReadByte();
					cmd.upmove = msg.ReadByte();
					cmd.angles[0] = msg.ReadShort();
					cmd.angles[1] = msg.ReadShort();
					cmd.angles[2] = msg.ReadShort();
					cmd.mx = msg.ReadShort();
					cmd.my = msg.ReadShort();
					cmd.impulse = msg.ReadByte();

					idVec3 _origin;
					idAngles _angles;
					_origin.x = msg.ReadFloat();
					_origin.y = msg.ReadFloat();
					_origin.z = msg.ReadFloat();
					_angles.yaw = msg.ReadFloat();
					_angles.pitch = 0;
					_angles.roll = 0;

					common->ServerSetUserCmdForClient(clientNum, cmd);
					player->SetOrigin(_origin);
					player->GetPhysics()->SetAxis(_angles.ToMat3());
					player->viewAngles = _angles;
					player->UpdateVisuals();
				}
				else {
					common->Warning("Recieved NET_OPCODE_UPDATEPLAYER from non-spawned clientnum %d\n", clientNum);
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
idGameLocal::ServerTeleportPlayer
====================
*/
void idGameLocal::ServerTeleportPlayer(int clientNum, idVec3 position, idMat3 axis) {
	rvmNetworkPacket teleportPacket(clientNum);
	idQuat quat;

	quat = axis.ToQuat();

	teleportPacket.msg.WriteUShort(NET_OPCODE_TELEPORTPLAYER);
	teleportPacket.msg.WriteByte(clientNum);
	teleportPacket.msg.WriteFloat(position.x);
	teleportPacket.msg.WriteFloat(position.y);
	teleportPacket.msg.WriteFloat(position.z);
	teleportPacket.msg.WriteFloat(quat.x);
	teleportPacket.msg.WriteFloat(quat.y);
	teleportPacket.msg.WriteFloat(quat.z);
	teleportPacket.msg.WriteFloat(quat.w);
	common->ServerSendReliableMessage(-1, teleportPacket.msg);
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

	// Teleport the player to the spawn point.
	idPlayer* player = entities[clientNum]->Cast<idPlayer>();
	ServerTeleportPlayer(clientNum, player->GetOrigin(), player->GetPhysics()->GetAxis());
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

	for (int i = 0; i < MAX_ASYNC_CLIENTS; i++) {
		if (entities[i] == NULL)
			continue;

		rvmNetworkPacket packet(gameLocal.localClientNum, 4096);
		packet.msg.WriteUShort(NET_OPCODE_UPDATEALLPLAYERS);

		int numClientsInPacket = 0;

		for (int clientNum = 0; clientNum < MAX_ASYNC_CLIENTS; clientNum++) {
			if (entities[clientNum] == NULL)
				continue;

			numClientsInPacket++;
		}

		packet.msg.WriteByte(numClientsInPacket);

		for (int clientNum = 0; clientNum < MAX_ASYNC_CLIENTS; clientNum++) {
			if (entities[clientNum] == NULL)
				continue;

			usercmd_t& cmd = gameLocal.usercmds[clientNum];
			idVec3 origin = gameLocal.entities[clientNum]->GetOrigin();
			idAngles rotation = gameLocal.entities[clientNum]->GetPhysics()->GetAxis().ToAngles();

			packet.msg.WriteByte(clientNum);
			packet.msg.WriteByte(cmd.forwardmove);
			packet.msg.WriteByte(cmd.rightmove);
			packet.msg.WriteByte(cmd.upmove);
			packet.msg.WriteShort(cmd.angles[0]);
			packet.msg.WriteShort(cmd.angles[1]);
			packet.msg.WriteShort(cmd.angles[2]);
			packet.msg.WriteShort(cmd.mx);
			packet.msg.WriteShort(cmd.my);
			packet.msg.WriteByte(cmd.impulse);
			packet.msg.WriteFloat(origin.x);
			packet.msg.WriteFloat(origin.y);
			packet.msg.WriteFloat(origin.z);
			packet.msg.WriteFloat(rotation.yaw);			
		}

		common->ServerSendReliableMessage(i, packet.msg);
	}

	snapShotSequence++;
}