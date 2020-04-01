// Model_fx.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "Model_fx.h"
#include "Model_local.h"

/*
========================
rvmRenderModelFX::rvmRenderModelFX
========================
*/
rvmRenderModelFX::rvmRenderModelFX() {

}

/*
========================
rvmRenderModelFX::~rvmRenderModelFX
========================
*/
rvmRenderModelFX::~rvmRenderModelFX() {
	for(int i = 0; i < effects.Num(); i++) {
		delete effects[i];
	}

	effects.Clear();
}

/*
========================
rvmRenderModelFX::ParseSimulation
========================
*/
void rvmRenderModelFX::ParseSimulation(const char* fileName, rvmEffectSimulation_t& simuation) {
	idParser src;
	idToken token;

	if(src.LoadFile(fileName) <= 0) {
		common->FatalError("ParseSimulation: Failed to load particle simulation %s\n", fileName);
		return;
	}

	// Parse the ident and version
	src.ExpectTokenString(PARTICLE_SIM_IDEN);
	src.ExpectTokenString(PARTICLE_SIM_VERSION);

	// Parse the frame count.
	src.ExpectTokenString("NumFrames");
	int numFrames = src.ParseInt();

	// Parse all the frames.
	simuation.frames.SetNum(numFrames);
	for(int i = 0; i < numFrames; i++) {
		rvmEffectSimFrame_t* frame = &simuation.frames[i];

		// Parse the particle frame ident.
		src.ExpectTokenString("ParticleFrame");
		src.ExpectTokenString("{");

		// Parse the particles.
		int numParticles = 0;
		src.ExpectTokenString("NumParticles");
		numParticles = src.ParseInt();

		// Parse each particle.
		for(int v = 0; v < numParticles; i++) {
			idVec3 point;

			point.x = src.ParseFloat();
			point.y = src.ParseFloat();
			point.z = src.ParseFloat();

			frame->points.Append(point);
		}

		// Ensure we read to the correct spot.
		src.ExpectTokenString("}");
	}
}

/*
========================
rvmRenderModelFX::ParseEffect
========================
*/
void rvmRenderModelFX::ParseEffect(rvmEffect_t* effect, idParser* src) {
	idToken token;

	src->ExpectTokenString("{");

	while (true) {
		if(src->EndOfFile()) {
			src->Error("ParseEffect: Unexpected EOF!\n");
		}

		src->ReadToken(&token);

		if(token == "}") {
			break;
		}
		else if (token == "simuation") {
			src->ReadToken(&token);

			ParseSimulation(token, effect->simulation);
		}		
		else {
			src->Error("ParseEffect: Unexpected token %s\n", token.c_str());
		}
	}
}

/*
========================
rvmRenderModelFX::InitFromFile
========================
*/
void rvmRenderModelFX::InitFromFile(const char* fileName) {
	idParser src;
	idToken token;

	if(parser.LoadFile(fileName) <= 0) {
		common->Warning("rvmRenderModelFX: Failed to load FX %s\n", fileName);
		return;
	}

	while (parser.ReadToken(&src)) {
		if (token == "effect") {
			rvmEffect_t* effect = new rvmEffect_t();

			src.ReadToken(&token);
			effect->name = token;

			ParseEffect(effect, &src);

			effects.Append(effect);
		}
		else {
			src.Error("Model FX unknown or unexpected keyword %s\n", token.c_str());
		}
	}
}

/*
========================
rvmRenderModelFX::IsDynamicModel
========================
*/
dynamicModel_t	rvmRenderModelFX::IsDynamicModel() const {
	return DM_CONTINUOUS;
}

/*
========================
rvmRenderModelFX::IsLoaded
========================
*/
bool rvmRenderModelFX::IsLoaded() const {
	return effects.Num() > 0;
}

/*
========================
rvmRenderModelFX::InstantiateDynamicModel
========================
*/
idRenderModel*  rvmRenderModelFX::InstantiateDynamicModel(const struct renderEntity_t* ent, const struct viewDef_s* view, idRenderModel* cachedModel) {
	return NULL;
}

/*
========================
rvmRenderModelFX::Bounds
========================
*/
idBounds rvmRenderModelFX::Bounds(const struct renderEntity_t* ent) const {
	return idBounds(-128, -128, -128, 128, 128, 128);
}