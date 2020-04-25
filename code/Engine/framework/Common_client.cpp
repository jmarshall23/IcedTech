// Common_client.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "Common_local.h"
#include "Common_network.h"
#include "Session_local.h"

/*
==================
idCommonLocal::ClientSendReliableMessage
==================
*/
void idCommonLocal::ClientSendReliableMessage(const idBitMsg& msg) {
	if (!IsDedicatedServer() && IsServer() && localClientNum != -1) {
		rvmNetworkPacketQueue_t* newPacket = new rvmNetworkPacketQueue_t(msg);
		newPacket->clientNum = -1;
		serverPacketQueue.AddToEnd(newPacket);
		return;
	}
}

int idCommonLocal::ClientGetTimeSinceLastPacket(void) {
	return 0;
}

void idCommonLocal::ExecuteClientMapChange(const char* mapName, const char* gameType) {

}