// Game_packet_client.cpp
//

#include "Game_precompiled.h"

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
				rvmNetworkPacket packet(NETWORK_PACKET_WRITE);

				packet.msg.WriteUShort(NET_OPCODE_SENDUSERINFO);
				packet.msg.WriteString(ui_name.GetString());
				common->ClientSendReliableMessage(packet.msg);
			}
			break;
		case NET_OPCODE_MAPCHANGE:
			{
				// We only need to change maps if we are a client, if we are a server, we already in the map.
				if(common->IsClient()) {
				
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
		default:
			common->Warning("ClientProcessPacket: Unknown OpCode %d\n", opCode);
			break;
	}
}

/*
================
idGameLocal::MapLoadFinished
================
*/
void idGameLocal::MapLoadFinished(void) {
	rvmNetworkPacket packet(NETWORK_PACKET_WRITE);
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

/*
================
idGameLocal::RunClientFrame
================
*/
void idGameLocal::RunClientFrame(void) {
	
}