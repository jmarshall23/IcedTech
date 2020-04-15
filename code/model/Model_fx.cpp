// Model_fx.cpp
//

#include "engine_precompiled.h"
#pragma hdrstop

#include "../renderer/tr_local.h"
#include "Model_fx.h"
#include "Model_local.h"

static const char* VFX_SnapshotName = "_VFX_Snapshot_";

/*
========================
rvmRenderModelFX::rvmRenderModelFX
========================
*/
rvmRenderModelFX::rvmRenderModelFX() {
	numFrames = 0;
	size = 1;
	normalizedAlpha = false;
	modelBounds.Clear();
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
rvmRenderModelFX::NumFrames
========================
*/
int rvmRenderModelFX::NumFrames() const {
	return numFrames;
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

	src.SetFlags(DECL_LEXER_FLAGS);

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
		for(int v = 0; v < numParticles; v++) {
			rvmEffectPoint_t point;

			point.xyz.x = src.ParseFloat();
			point.xyz.y = src.ParseFloat();
			point.xyz.z = src.ParseFloat();

			modelBounds.AddPoint(point.xyz);

			src.ReadToken(&token);

			if (token == "ALIVE") {
				point.state = PARTICLE_STATE_ALIVE;
			}
			else if(token == "UNBORN") {
				point.state = PARTICLE_STATE_UNBORN;
			}
			else if(token == "DEAD") {
				point.state = PARTICLE_STATE_DEAD;
			}
			else {
				src.Error("Unknown particle state %s\n", token.c_str());
			}

			point.normalized_time = src.ParseFloat() / (float)numFrames;
			point.size = src.ParseFloat();

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
		else if(token == "material") {
			src->ReadToken(&token);
			effect->mtr = declManager->FindMaterial(token, false);
		}
		else if(token == "size") {
			size = src->ParseInt();
		}
		else if (token == "sim_scale") {
			effect->sim_scale = src->ParseFloat();
		}
		else if(token == "normalizedAlpha") {
			normalizedAlpha = true;
		}
		else {
			src->Error("ParseEffect: Unexpected token %s\n", token.c_str());
		}
	}
}

/*
========================
rvmRenderModelFX::LoadModel
========================
*/
void rvmRenderModelFX::LoadModel(void) {
	idParser src;
	idToken token;

	if (src.LoadFile(name) <= 0) {
		common->Warning("rvmRenderModelFX: Failed to load FX %s\n", name.c_str());
		return;
	}

	src.SetFlags(DECL_LEXER_FLAGS);

	while (src.ReadToken(&token)) {
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

	// Number of frames is based on the effect with the most simulation frames.
	for(int i = 0; i < effects.Num(); i++) {
		if(numFrames < effects[i]->simulation.frames.Num()) {
			numFrames = effects[i]->simulation.frames.Num() - 1;
		}
	}
}

/*
========================
rvmRenderModelFX::InitFromFile
========================
*/
void rvmRenderModelFX::InitFromFile(const char* fileName) {
	name = fileName;
	LoadModel();
}

/*
========================
rvmRenderModelFX::IsDynamicModel
========================
*/
dynamicModel_t	rvmRenderModelFX::IsDynamicModel() const {
	return DM_CACHED;
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
rvmRenderModelFX::CreateParticle
========================
*/
void rvmRenderModelFX::CreateParticle(rvmEffectPoint_t &point, float simScale, idMat3 axis, int size, idDrawVert* verts) const {
	idVec3	left, up;
	idVec3	entityLeft, entityUp;
	float	s, width;
	float	t, height;

	s = 0.0f;
	width = 1.0f;
	t = 0.0f;
	height = 1.0f;

	float	angle;
	angle = 360 * idMath::FRand();

	angle = angle / 180 * idMath::PI;

	{
		float c = idMath::Cos16(angle);
		float s = idMath::Sin16(angle);

		axis.ProjectVector(tr.viewDef->renderView.viewaxis[1], entityLeft);
		axis.ProjectVector(tr.viewDef->renderView.viewaxis[2], entityUp);

		left = entityLeft * c + entityUp * s;
		up = entityUp * c - entityLeft * s;

		left *= (point.size * size);
		up *= (point.size * size);
	}

	// Assign the vertex coords.
	verts[0].xyz = (point.xyz * simScale) - left + up;
	verts[1].xyz = (point.xyz * simScale) + left + up;
	verts[2].xyz = (point.xyz * simScale) - left - up;
	verts[3].xyz = (point.xyz * simScale) + left - up;

	// Assign the texture coords.
	verts[0].st[0] = s;
	verts[0].st[1] = t;
	verts[1].st[0] = s + width;
	verts[1].st[1] = t;
	verts[2].st[0] = s;
	verts[2].st[1] = t + height;
	verts[3].st[0] = s + width;
	verts[3].st[1] = t + height;

	// Assign the vertex color.
	if(point.state != PARTICLE_STATE_ALIVE) {
		for (int i = 0; i < 4; i++) {
			verts[i].color[0] = 0;
			verts[i].color[1] = 0;
			verts[i].color[2] = 0;
			verts[i].color[3] = 0;
		}
	}
	else {
		if(normalizedAlpha) {
			for (int i = 0; i < 4; i++) {
				verts[i].color[0] = 0;
				verts[i].color[1] = 0;
				verts[i].color[2] = 0;
				verts[i].color[3] = (byte)(point.normalized_time * 255.0f);
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				verts[i].color[0] = 0;
				verts[i].color[1] = 0;
				verts[i].color[2] = 0;
				verts[i].color[3] = 255;
			}
		}		
	}
}

/*
========================
rvmRenderModelFX::ProcessEffectGeometry
========================
*/
void rvmRenderModelFX::ProcessEffectGeometry(int frameNum, idMat3 axis, modelSurface_t* surf, rvmEffect_t* effect) {
	int i, base;
	srfTriangles_t* tri;
	int numIndexes, numVertexes;
	rvmEffectSimFrame_t* frame = &effect->simulation.frames[frameNum];

	// Set how many vertexes and indexes we have for this effect.
	numVertexes = frame->points.Num() * 4;
	numIndexes = frame->points.Num() * 6;

	surf->shader = effect->mtr;

	if (surf->geometry) {
		// if the number of verts and indexes are the same we can re-use the triangle surface
		// the number of indexes must be the same to assure the correct amount of memory is allocated for the facePlanes
		if (surf->geometry->numVerts == numVertexes && surf->geometry->numIndexes == numIndexes) {
			R_FreeStaticTriSurfVertexCaches(surf->geometry);
		}
		else {
			R_FreeStaticTriSurf(surf->geometry);
			surf->geometry = R_AllocStaticTriSurf();
			R_AllocStaticTriSurfVerts(surf->geometry, numVertexes);
			R_AllocStaticTriSurfIndexes(surf->geometry, numIndexes);
			R_AllocStaticTriSurfPlanes(surf->geometry, numIndexes);
		}
	}
	else {
		surf->geometry = R_AllocStaticTriSurf();
		R_AllocStaticTriSurfVerts(surf->geometry, numVertexes);
		R_AllocStaticTriSurfIndexes(surf->geometry, numIndexes);
		R_AllocStaticTriSurfPlanes(surf->geometry, numIndexes);
	}

	surf->geometry->bounds.Clear();

	// Create all of the particle vertexes.
	for (int i = 0; i < frame->points.Num(); i++) {
		float particle_size = size;

		CreateParticle(frame->points[i], effect->sim_scale, axis, particle_size, &surf->geometry->verts[i * 4]);
		surf->geometry->bounds.AddPoint(frame->points[i].xyz);
	}

	// Create all of the particle indexes.
	int	indexId = 0;
	glIndex_t* indexes = surf->geometry->indexes;
	for (int i = 0; i < numVertexes; i += 4) {
		indexes[indexId + 0] = i;
		indexes[indexId + 1] = i + 2;
		indexes[indexId + 2] = i + 3;
		indexes[indexId + 3] = i;
		indexes[indexId + 4] = i + 3;
		indexes[indexId + 5] = i + 1;
		indexId += 6;
	}

	tri = surf->geometry;

	// note that some of the data is references, and should not be freed
	tri->deformedSurface = false;
	tri->tangentsCalculated = true;
	tri->facePlanesCalculated = false;

	tri->numIndexes = indexId;
	tri->silIndexes = 0;
	tri->numMirroredVerts = 0;
	tri->mirroredVerts = 0;
	tri->numDupVerts = 0;
	tri->dupVerts = 0;
	tri->numSilEdges = 0;
	tri->silEdges = 0;
	tri->dominantTris = 0;
	tri->numVerts = numVertexes;
}

/*
========================
rvmRenderModelFX::InstantiateDynamicModel
========================
*/
idRenderModel*  rvmRenderModelFX::InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s* view, idRenderModel* cachedModel) {
	idRenderModelStatic* staticModel;

	if (cachedModel && !r_useCachedDynamicModels.GetBool()) {
		delete cachedModel;
		cachedModel = NULL;
	}

	const idRenderEntityParms* parms = &((const idRenderEntityLocal*)ent)->parms;

	if (purged) {
		common->DWarning("model %s instantiated while purged", Name());
		LoadModel();
	}

	if (cachedModel) {
		assert(dynamic_cast<idRenderModelStatic*>(cachedModel) != NULL);
		assert(idStr::Icmp(cachedModel->Name(), VFX_SnapshotName) == 0);
		staticModel = static_cast<idRenderModelStatic*>(cachedModel);
	}
	else {
		staticModel = new idRenderModelStatic;
		staticModel->InitEmpty(VFX_SnapshotName);
	}

	for(int i = 0; i < effects.Num(); i++) {
		modelSurface_t* surf;
		int surfaceNum;

		if (staticModel->FindSurfaceWithId(i, surfaceNum)) {
			surf = &staticModel->surfaces[surfaceNum];
		}
		else {
			surf = &staticModel->surfaces.Alloc();
			surf->geometry = NULL;
			surf->shader = NULL;
			surf->id = i;
		}

		ProcessEffectGeometry(parms->frameNum, parms->axis, surf, effects[i]);

		staticModel->bounds.AddPoint(surf->geometry->bounds[0]);
		staticModel->bounds.AddPoint(surf->geometry->bounds[1]);
	}

	return staticModel;
}

/*
========================
rvmRenderModelFX::Bounds
========================
*/
idBounds rvmRenderModelFX::Bounds(const class idRenderEntity* ent) const {
	return modelBounds;
}