// Game_packet_client.cpp
//

#include "Game_precompiled.h"
#include "Client_local.h"

extern idCVar ui_name;

/*
=======================
idGameLocal::ClientProcessPacket
=======================
*/
void idGameLocal::ClientProcessPacket(int clientNum, const idBitMsg& msg) {
	int opCode = msg.ReadUShort();

	switch(opCode) {
		case NET_OPCODE_GETUSERINFO:
			{
				rvmNetworkPacket packet(clientNum);

				packet.msg.WriteUShort(NET_OPCODE_SENDUSERINFO);
				packet.msg.WriteString(ui_name.GetString());
				common->ClientSendReliableMessage(packet.msg);
			}
			break;
		case NET_OPCODE_MAPCHANGE:
			{
				// We only need to change maps if we are a client, if we are a server, we already in the map.
				if(common->IsClient()) {
					char mapName[256];
					char gameType[256];

					msg.ReadString(mapName, sizeof(mapName));
					msg.ReadString(gameType, sizeof(gameType));

					common->ExecuteClientMapChange(mapName, gameType);
				}
				else {
					MapLoadFinished();
				}
			}
			break;
		case NET_OPCODE_INIT_DECL_REMAP:
			InitClientDeclRemap(clientNum);
			break;
		case NET_OPCODE_SPAWNPLAYER:
			{
				int client = msg.ReadByte();
				int spawnId = msg.ReadLong();
				if (!entities[client]) {
					SpawnPlayer(client, false, NULL);
				}
				// fix up the spawnId to match what the server says
				// otherwise there is going to be a bogus delete/new of the client entity in the first ClientReadFromSnapshot
				spawnIds[client] = spawnId;
			}
			break;
		case NET_OPCODE_CHATMESSAGE:
			{
				char name[128];
				char text[128];
				msg.ReadString(name, sizeof(name));
				msg.ReadString(text, sizeof(text));
				mpGame.AddChatLine("%s^0: %s\n", name, text);
			}
			break;
		case NET_OPCODE_SNAPSHOT:
			ClientReadSnapshot(localClientNum, snapShotSequence, common->GetGameFrame(), gameLocal.time, 0, 0, msg);
			snapShotSequence++;
			break;
		case NET_OPCODE_UPDATEALLPLAYERS:
			{
				int numClientsInPacket = msg.ReadByte();

				for (int d = 0; d < numClientsInPacket; d++) {
					int _clientNum = msg.ReadByte();
					
					usercmd_t cmd;
					memset(&cmd, 0, sizeof(usercmd_t));

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
					idQuat _quat;
					_origin.x = msg.ReadFloat();
					_origin.y = msg.ReadFloat();
					_origin.z = msg.ReadFloat();
					_quat.x = msg.ReadFloat();
					_quat.y = msg.ReadFloat();
					_quat.z = msg.ReadFloat();
					_quat.w = msg.ReadFloat();
					
					//if (_clientNum != clientNum) {
						idPlayer* player = entities[_clientNum]->Cast<idPlayer>();

						if (player != NULL) {
							//common->ServerSetUserCmdForClient(_clientNum, cmd);
							gameLocal.usercmds[_clientNum] = cmd;
							player->Spectate(false);
							player->SetOrigin(_origin);
							player->SetAxis(_quat.ToMat3());
						}
					//}
				}
			}
			break;
		case NET_OPCODE_SPECTATE:
			{
				int _clientNum = msg.ReadByte();
				bool _spectate = msg.ReadByte() != 0;
				entities[_clientNum]->Cast<idPlayer>()->Spectate(_spectate);
			}
			break;
		case NET_OPCODE_TELEPORTPLAYER:
			{
				int _clientNum;
				idVec3 _position;
				idQuat _quat;

				_clientNum = msg.ReadByte();
				_position.x = msg.ReadFloat();
				_position.y = msg.ReadFloat();
				_position.z = msg.ReadFloat();

				_quat.x = msg.ReadFloat();
				_quat.y = msg.ReadFloat();
				_quat.z = msg.ReadFloat();
				_quat.w = msg.ReadFloat();

				entities[_clientNum]->Cast<idPlayer>()->SetOrigin(_position);
				entities[_clientNum]->Cast<idPlayer>()->GetPhysics()->SetAxis(_quat.ToMat3());
			}
			break;
		case NET_OPCODE_SPAWNDEBRIS:
			clientLocal.NetLaunchDebris(msg);
			break;
		default:
			common->Warning("ClientProcessPacket: Unknown OpCode %d\n", opCode);
			break;
	}
}

/*
================
rvmClientGameLocal::NetLaunchDebris
================
*/
void rvmClientGameLocal::NetLaunchDebris(const idBitMsg& msg) {
	idVec3 _origin;
	idMat3 _axis;
	idQuat _quat;
	char _shaderName[256];

	_origin.x = msg.ReadFloat();
	_origin.y = msg.ReadFloat();
	_origin.z = msg.ReadFloat();

	_quat.x = msg.ReadFloat();
	_quat.y = msg.ReadFloat();
	_quat.z = msg.ReadFloat();
	_quat.w = msg.ReadFloat();

	_axis = _quat.ToMat3();

	msg.ReadString(_shaderName, sizeof(_shaderName));

	rvmClientEffect_debris* entity;
	idDict args;

	args.Set("classname", "wall_debris");
	args.Set("origin", _origin.ToString());

	clientLocal.SpawnClientEntityDef(args, (rvClientEntity**)(&entity), false, "rvmClientEffect_debris");
	entity->LaunchEffect(debrisEntityDef, DEBRIS_MODEL_COUNT, _origin, _axis, _shaderName);
}

/*
================
idGameLocal::MapLoadFinished
================
*/
void idGameLocal::MapLoadFinished(void) {
	clientLocal.BeginLevel();

	rvmNetworkPacket packet(common->GetLocalClientNum());
	packet.msg.WriteShort(NET_OPCODE_CLIENTLOADED);
	common->ClientSendReliableMessage(packet.msg);
}

/*
================
idGameLocal::ClientNetSendUserCmd
================
*/
void idGameLocal::ClientNetSendUserCmd(void) {

}
