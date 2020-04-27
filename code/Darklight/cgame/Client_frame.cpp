// Client_frame.cpp
//

#include "Game_precompiled.h"
#include "Client_local.h"

/*
================
idGameLocal::RunClientFrame
================
*/
void idGameLocal::RunClientFrame(void) {
	clientLocal.clientPhysicsJob->Wait();
	clientLocal.clientEntityThreadWork.Clear();


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