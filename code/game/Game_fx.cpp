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
	emitter->renderEntity.hModel = (idRenderModel *)emitter->fxModel;
	emitter->renderEntity.lightChannel = 0;
	emitter->renderEntity.origin = origin;
	emitter->renderEntity.axis = axis;
	emitter->renderEntity.frameNum = 0;
	emitter->renderEntity.bounds = emitter->fxModel->Bounds();
	emitter->renderEntity.entityNum = 1;
	emitter->renderEntity.SetLightChannel(LIGHT_CHANNEL_WORLD, true);

	emitter->worldHandle = gameRenderWorld->AddEntityDef(&emitter->renderEntity);

	fxEmitters.AddToFront(emitter);

	return emitter;
}

/*
==================
idGameLocal::RemoveEffect
==================
*/
void idGameLocal::RemoveEffect(fxEmitterInstance_t* fx) {
	gameRenderWorld->FreeEntityDef(fx->worldHandle);
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

		emitter->renderEntity.frameNum++;
		if(emitter->renderEntity.frameNum >= emitter->fxModel->NumFrames()) {	
			if(emitter->loop)
			{
				emitter->renderEntity.frameNum = 0;
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
			gameRenderWorld->UpdateEntityDef(emitter->worldHandle, &emitter->renderEntity);
		}

		emitter = emitter->listNode.GetNext();
	}
}