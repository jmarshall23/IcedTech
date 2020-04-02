// Game_fx.cpp
//

#include "Game_precompiled.h"

/*
==================
idGameLocal::CreateEffect
==================
*/
void idGameLocal::CreateEffect(const char* effectModel, idVec3 origin, idMat3 axis, bool loop) {
	fxEmitterInstance_t emitter;

	memset(&emitter, 0, sizeof(fxEmitterInstance_t));

	emitter.loop = loop;
	emitter.fxModel = renderModelManager->FindModel(effectModel);
	emitter.renderEntity.hModel = (idRenderModel *)emitter.fxModel;
	emitter.renderEntity.lightChannel = 0;
	emitter.renderEntity.origin = origin;
	emitter.renderEntity.axis = axis;
	emitter.renderEntity.frameNum = 0;
	emitter.renderEntity.bounds = emitter.fxModel->Bounds();
	emitter.renderEntity.entityNum = 1;
	emitter.renderEntity.SetLightChannel(LIGHT_CHANNEL_WORLD, true);

	emitter.worldHandle = gameRenderWorld->AddEntityDef(&emitter.renderEntity);

	fxEmitters.Append(emitter);
}

/*
==================
idGameLocal::RunFX
==================
*/
void idGameLocal::RunFX(void) {
	for(int i = 0; i < fxEmitters.Num(); i++) {
		fxEmitterInstance_t* emitter = &fxEmitters[i];

		emitter->renderEntity.frameNum++;
		if(emitter->renderEntity.frameNum >= emitter->fxModel->NumFrames()) {	
			if(emitter->loop)
			{
				emitter->renderEntity.frameNum = 0;
			}
			else
			{
				gameRenderWorld->FreeEntityDef(emitter->worldHandle);
				fxEmitters.RemoveIndex(i);
			}
			break;
		}
		else {
			gameRenderWorld->UpdateEntityDef(emitter->worldHandle, &emitter->renderEntity);
		}
	}
}