// Client_frame.cpp
//

#include "Game_precompiled.h"
#include "Client_local.h"

/*
================
idGameLocal::RunClientFrame
================
*/
void idGameLocal::RunClientFrame(usercmd_t& cmd) {
	clientLocal.clientPhysicsJob->Wait();
	clientLocal.clientEntityThreadWork.Clear();

	for(int i = 0; i < clientLocal.activeClientEntities.Num(); i++) {
		clientLocal.activeClientEntities[i]->Think();
	}

	// Write our local player info to the server.
	if(!common->IsDedicatedServer() && gameLocal.localClientNum != -1 && !common->IsServer()) {
		if (gameLocal.entities[gameLocal.localClientNum] != NULL)
		{
			idVec3 origin = gameLocal.entities[gameLocal.localClientNum]->GetOrigin();
			idQuat rotation = gameLocal.entities[gameLocal.localClientNum]->GetPhysics()->GetAxis().ToQuat();

			rvmNetworkPacket packet(gameLocal.localClientNum);
			packet.msg.WriteUShort(NET_OPCODE_UPDATEPLAYER);
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
			packet.msg.WriteFloat(rotation.y);
			packet.msg.WriteFloat(rotation.z);
			packet.msg.WriteFloat(rotation.z);
			packet.msg.WriteFloat(rotation.w);
			//gameLocal.entities[gameLocal.localClientNum]->Cast<idPlayer>()->GetPhysics()->WriteToSnapshot(packet.msg);
			common->ClientSendReliableMessage(packet.msg);
		}
	}

	// Submit any work we are going to reap the next frame.
	{
		static rvmClientPhysicsJobParams_t clientPhysicsJob1Params;
		//static rvmClientPhysicsJobParams_t clientPhysicsJob2Params;

		clientPhysicsJob1Params.clientThreadId = ENTITYNUM_CLIENT;
		clientPhysicsJob1Params.startClientEntity = 0;
		clientPhysicsJob1Params.numClientEntities = clientLocal.clientEntityThreadWork.Num();

		//clientPhysicsJob1Params.clientThreadId = ENTITYNUM_CLIENT2;
		//clientPhysicsJob2Params.startClientEntity = clientPhysicsJob1Params.numClientEntities;
		//clientPhysicsJob2Params.numClientEntities = gameLocal.clientEntityThreadWork.Num();

		clientLocal.clientPhysicsJob->AddJobA((jobRun_t)rvmClientGameLocal::ClientEntityJob_t, &clientPhysicsJob1Params);
		//clientPhysicsJob->AddJobA((jobRun_t)idGameLocal::ClientEntityJob_t, &clientPhysicsJob2Params);
		clientLocal.clientPhysicsJob->Submit();
	}
}