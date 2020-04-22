// Game_fx.cpp
//

#include "Game_precompiled.h"

/*
==================
idGameLocal::CreateEffect
==================
*/
fxEmitterInstance_t *idGameLocal::CreateEffect(const char* effectModel, idVec3 origin, idMat3 axis, bool loop) {
	if (effectModel == NULL || effectModel[0] == '\0')
		return NULL;

	fxEmitterInstance_t* emitter = new fxEmitterInstance_t();
	memset(emitter, 0, sizeof(fxEmitterInstance_t));

	emitter->loop = loop;
	emitter->fxModel = renderModelManager->FindModel(effectModel);

	emitter->renderEntity = gameRenderWorld->AllocRenderEntity();
	emitter->renderEntity->SetRenderModel((idRenderModel *)emitter->fxModel);
	emitter->renderEntity->SetLightChannel(LIGHT_CHANNEL_WORLD, true);
	emitter->renderEntity->SetOrigin(origin);
	emitter->renderEntity->SetAxis(axis);
	emitter->renderEntity->SetFrameNum(0);
	emitter->renderEntity->SetBounds(emitter->fxModel->Bounds());
	emitter->renderEntity->SetEntityNum(1);
	
	fxEmitters.AddToFront(emitter);

	return emitter;
}

/*
==================
idGameLocal::RemoveEffect
==================
*/
void idGameLocal::RemoveEffect(fxEmitterInstance_t* fx) {
	gameRenderWorld->FreeRenderEntity(fx->renderEntity);
	fx->renderEntity = NULL;
	fxEmitters.Remove(fx);
	delete fx;
}

/*
==================
idGameLocal::RunFX
==================
*/
void idGameLocal::RunFX(void) {
	fxEmitterInstance_t* emitter = fxEmitters.GetFirst();

	while(true) {
		if (emitter == NULL)
			break;

		int frameNum = emitter->renderEntity->GetFrameNum();

		frameNum++;
		if(frameNum >= emitter->fxModel->NumFrames()) {
			if(emitter->loop)
			{
				frameNum = 0;
			}
			else {
				fxEmitterInstance_t* oldEmitter = emitter;
				emitter = emitter->listNode.GetNext();
				RemoveEffect(oldEmitter);
				continue;
			}
			break;
		}
		else {
			emitter->renderEntity->SetFrameNum(frameNum);
			emitter->renderEntity->UpdateRenderEntity();
		}

		emitter = emitter->listNode.GetNext();
	}
}