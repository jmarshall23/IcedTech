// Model_mdr.cpp
//

#include "Engine_precompiled.h"
#include "tr_local.h"
#include "Model_local.h"
#include "Model_mdr.h"

/*
========================
rvmRenderModelMDR::rvmRenderModelMDR
========================
*/
rvmRenderModelMDR::rvmRenderModelMDR() {
	dataSize = 0;
	modelData = nullptr;
	cachedModel = nullptr;
}

/*
========================
rvmRenderModelMDR::~rvmRenderModelMDR
========================
*/
rvmRenderModelMDR::~rvmRenderModelMDR() {
	if(modelData)
	{
		Mem_Free(modelData);
		modelData = nullptr;
	}

	if (cachedModel)
	{
		delete cachedModel;
		cachedModel = nullptr;
	}

	dataSize = 0;
}

/*
========================
rvmRenderModelMDR::InitFromFile
========================
*/
void rvmRenderModelMDR::InitFromFile(const char *fileName) {
	name = fileName;
	LoadModel();
}

/*
========================
rvmRenderModelMDR::LoadModel
========================
*/
void rvmRenderModelMDR::LoadModel(void) {
	void *mdrMeshBuffer;
	int mdrMeshLen;

	if (!purged) {
		PurgeModel();
	}
	purged = false;

	materials.Clear();

	// Try and load the file off disk.
	mdrMeshLen = fileSystem->ReadFile(name, &mdrMeshBuffer);
	if (mdrMeshLen <= 0 || mdrMeshBuffer == nullptr)
	{
		common->Warning("Failed to load MDR mesh %s\n", name.c_str());
		MakeDefaultModel();
		return;
	}

	// Parse the MDR file.
	if (!LoadMDRFile(mdrMeshBuffer, mdrMeshLen))
	{
		MakeDefaultModel();
		fileSystem->FreeFile(mdrMeshBuffer);
		return;
	}

	// If we don't have any frames, then render as a static mesh.
	//if (modelData->numFrames == 1)
	{
		viewDef_t view;
		RenderFramesToModel(this, NULL, &view, NULL, 1);
	}

	// Free the memory from reading in the ifle.
	fileSystem->FreeFile(mdrMeshBuffer);
}

/*
========================
rvmRenderModelMDR::LoadMDRFile
========================
*/
bool rvmRenderModelMDR::LoadMDRFile(void *buffer, int filesize) {
	int					i, j, k, l;
	mdrHeader_t			*pinmodel, *mdr;
	mdrFrame_t			*frame;
	mdrLOD_t			*lod, *curlod;
	mdrSurface_t			*surf, *cursurf;
	mdrTriangle_t			*tri, *curtri;
	mdrVertex_t			*v, *curv;
	mdrWeight_t			*weight, *curweight;
	mdrTag_t			*tag, *curtag;
	int					size;
	const idMaterial	*sh;

	const char *mod_name = name.c_str();

	idStr shaderPrefix = name;
	shaderPrefix.StripFileExtension();

	bounds.Clear();

	pinmodel = (mdrHeader_t *)buffer;

	pinmodel->version = LittleLong(pinmodel->version);
	if (pinmodel->version != MDR_VERSION)
	{
		common->Warning("R_LoadMDR: %s has wrong version (%i should be %i)\n", mod_name, pinmodel->version, MDR_VERSION);
		return false;
	}

	size = LittleLong(pinmodel->ofsEnd);

	if (size > filesize)
	{
		common->Warning("R_LoadMDR: Header of %s is broken. Wrong filesize declared!\n", mod_name);
		return false;
	}

	LL(pinmodel->numFrames);
	LL(pinmodel->numBones);
	LL(pinmodel->ofsFrames);

	// This is a model that uses some type of compressed Bones. We don't want to uncompress every bone for each rendered frame
	// over and over again, we'll uncompress it in this function already, so we must adjust the size of the target mdr.
	if (pinmodel->ofsFrames < 0)
	{
		// mdrFrame_t is larger than mdrCompFrame_t:
		size += pinmodel->numFrames * sizeof(frame->name);
		// now add enough space for the uncompressed bones.
		size += pinmodel->numFrames * pinmodel->numBones * ((sizeof(mdrBone_t) - sizeof(mdrCompBone_t)));
	}

	// simple bounds check
	if (pinmodel->numBones < 0 ||
		sizeof(*mdr) + pinmodel->numFrames * (sizeof(*frame) + (pinmodel->numBones - 1) * sizeof(*frame->bones)) > size)
	{
		common->Warning("R_LoadMDR: %s has broken structure.\n", mod_name);
		return false;
	}

	dataSize += size;
	modelData = mdr = (mdrHeader_t *)Mem_Alloc(size);

	// Copy all the values over from the file and fix endian issues in the process, if necessary.

	mdr->ident = LittleLong(pinmodel->ident);
	mdr->version = pinmodel->version;	// Don't need to swap byte order on this one, we already did above.
	strcpy(mdr->name, pinmodel->name);
	mdr->numFrames = pinmodel->numFrames;
	mdr->numBones = pinmodel->numBones;
	mdr->numLODs = LittleLong(pinmodel->numLODs);
	mdr->numTags = LittleLong(pinmodel->numTags);
	// We don't care about the other offset values, we'll generate them ourselves while loading.

	numLods = mdr->numLODs;

	if (mdr->numFrames < 1)
	{
		common->Warning("R_LoadMDR: %s has no frames\n", mod_name);
		return false;
	}

	/* The first frame will be put into the first free space after the header */
	frame = (mdrFrame_t *)(mdr + 1);
	mdr->ofsFrames = (int)((byte *)frame - (byte *)mdr);

	if (pinmodel->ofsFrames < 0)
	{
		mdrCompFrame_t *cframe;

		// compressed model...				
		cframe = (mdrCompFrame_t *)((byte *)pinmodel - pinmodel->ofsFrames);

		for (i = 0; i < mdr->numFrames; i++)
		{
			for (j = 0; j < 3; j++)
			{
				frame->bounds[0][j] = LittleFloat(cframe->bounds[0][j]);
				frame->bounds[1][j] = LittleFloat(cframe->bounds[1][j]);
				frame->localOrigin[j] = LittleFloat(cframe->localOrigin[j]);
			}

			if (i == 0) {
				bounds[0] = frame->bounds[0];
				bounds[1] = frame->bounds[1];
			}

			frame->radius = LittleFloat(cframe->radius);
			frame->name[0] = '\0';	// No name supplied in the compressed version.

			for (j = 0; j < mdr->numBones; j++)
			{
				for (k = 0; k < (sizeof(cframe->bones[j].Comp) / 2); k++)
				{
					// Do swapping for the uncompressing functions. They seem to use shorts
					// values only, so I assume this will work. Never tested it on other
					// platforms, though.

					((unsigned short *)(cframe->bones[j].Comp))[k] =
						LittleShort(((unsigned short *)(cframe->bones[j].Comp))[k]);
				}

				/* Now do the actual uncompressing */
				MC_UnCompress(frame->bones[j].matrix, cframe->bones[j].Comp);
			}

			// Next Frame...
			cframe = (mdrCompFrame_t *)&cframe->bones[j];
			frame = (mdrFrame_t *)&frame->bones[j];
		}
	}
	else
	{
		mdrFrame_t *curframe;

		// uncompressed model...
		//

		curframe = (mdrFrame_t *)((byte *)pinmodel + pinmodel->ofsFrames);

		// swap all the frames
		for (i = 0; i < mdr->numFrames; i++)
		{
			for (j = 0; j < 3; j++)
			{
				frame->bounds[0][j] = LittleFloat(curframe->bounds[0][j]);
				frame->bounds[1][j] = LittleFloat(curframe->bounds[1][j]);
				frame->localOrigin[j] = LittleFloat(curframe->localOrigin[j]);
			}

			if(i == 0) {
				bounds[0] = frame->bounds[0];
				bounds[1] = frame->bounds[1];
			}

			frame->radius = LittleFloat(curframe->radius);
			strcpy(frame->name, curframe->name);

			for (j = 0; j < (int)(mdr->numBones * sizeof(mdrBone_t) / 4); j++)
			{
				((float *)frame->bones)[j] = LittleFloat(((float *)curframe->bones)[j]);
			}

			curframe = (mdrFrame_t *)&curframe->bones[mdr->numBones];
			frame = (mdrFrame_t *)&frame->bones[mdr->numBones];
		}
	}

	// frame should now point to the first free address after all frames.
	lod = (mdrLOD_t *)frame;
	mdr->ofsLODs = (int)((byte *)lod - (byte *)mdr);

	curlod = (mdrLOD_t *)((byte *)pinmodel + LittleLong(pinmodel->ofsLODs));

	// swap all the LOD's
	for (l = 0; l < mdr->numLODs; l++)
	{
		// simple bounds check
		if ((byte *)(lod + 1) > (byte *) mdr + size)
		{
			common->Warning("R_LoadMDR: %s has broken structure.\n", mod_name);
			return false;
		}

		lod->numSurfaces = LittleLong(curlod->numSurfaces);

		// swap all the surfaces
		surf = (mdrSurface_t *)(lod + 1);
		lod->ofsSurfaces = (int)((byte *)surf - (byte *)lod);
		cursurf = (mdrSurface_t *)((byte *)curlod + LittleLong(curlod->ofsSurfaces));

		for (i = 0; i < lod->numSurfaces; i++)
		{
			// simple bounds check
			if ((byte *)(surf + 1) > (byte *) mdr + size)
			{
				common->Warning("R_LoadMDR: %s has broken structure.\n", mod_name);
				return false;
			}

			// first do some copying stuff
			strcpy(surf->name, cursurf->name);
			strcpy(surf->shader, cursurf->shader);

			surf->ofsHeader = (byte *)mdr - (byte *)surf;

			surf->numVerts = LittleLong(cursurf->numVerts);
			surf->numTriangles = LittleLong(cursurf->numTriangles);
			// numBoneReferences and BoneReferences generally seem to be unused

			// lowercase the surface name so skin compares are faster
			strlwr(surf->name);

			// register the shaders
			idStr materialName = va("%s/%s", shaderPrefix.c_str(), surf->shader);
			materialName.Replace(" ", "_");
			materialName.Replace("-", "_");
			sh = declManager->FindMaterial(materialName, true); //R_FindShader(surf->shader, LIGHTMAP_NONE, qtrue);
			if (sh != nullptr)
			{
				surf->shaderIndex = sh->Index();
			}
			else
			{
				surf->shaderIndex = 0;
				common->Warning("Failed to find material %s\n", materialName);
			}

			materials.Append(sh);

			// now copy the vertexes.
			v = (mdrVertex_t *)(surf + 1);
			surf->ofsVerts = (int)((byte *)v - (byte *)surf);
			curv = (mdrVertex_t *)((byte *)cursurf + LittleLong(cursurf->ofsVerts));

			for (j = 0; j < surf->numVerts; j++)
			{
				LL(curv->numWeights);

				// simple bounds check
				if (curv->numWeights < 0 || (byte *)(v + 1) + (curv->numWeights - 1) * sizeof(*weight) >(byte *) mdr + size)
				{
					common->Warning("R_LoadMDR: %s has broken structure.\n", mod_name);
					return false;
				}

				v->normal[0] = LittleFloat(curv->normal[0]);
				v->normal[1] = LittleFloat(curv->normal[1]);
				v->normal[2] = LittleFloat(curv->normal[2]);

				v->texCoords[0] = LittleFloat(curv->texCoords[0]);
				v->texCoords[1] = LittleFloat(curv->texCoords[1]);

				v->numWeights = curv->numWeights;
				weight = &v->weights[0];
				curweight = &curv->weights[0];

				// Now copy all the weights
				for (k = 0; k < v->numWeights; k++)
				{
					weight->boneIndex = LittleLong(curweight->boneIndex);
					weight->boneWeight = LittleFloat(curweight->boneWeight);

					weight->offset[0] = LittleFloat(curweight->offset[0]);
					weight->offset[1] = LittleFloat(curweight->offset[1]);
					weight->offset[2] = LittleFloat(curweight->offset[2]);

					weight++;
					curweight++;
				}

				v = (mdrVertex_t *)weight;
				curv = (mdrVertex_t *)curweight;
			}

			// we know the offset to the triangles now:
			tri = (mdrTriangle_t *)v;
			surf->ofsTriangles = (int)((byte *)tri - (byte *)surf);
			curtri = (mdrTriangle_t *)((byte *)cursurf + LittleLong(cursurf->ofsTriangles));

			// simple bounds check
			if (surf->numTriangles < 0 || (byte *)(tri + surf->numTriangles) >(byte *) mdr + size)
			{
				common->Warning("R_LoadMDR: %s has broken structure.\n", mod_name);
				return false;
			}

			for (j = 0; j < surf->numTriangles; j++)
			{
				tri->indexes[0] = LittleLong(curtri->indexes[0]);
				tri->indexes[1] = LittleLong(curtri->indexes[1]);
				tri->indexes[2] = LittleLong(curtri->indexes[2]);

				tri++;
				curtri++;
			}

			// tri now points to the end of the surface.
			surf->ofsEnd = (byte *)tri - (byte *)surf;
			surf = (mdrSurface_t *)tri;

			// find the next surface.
			cursurf = (mdrSurface_t *)((byte *)cursurf + LittleLong(cursurf->ofsEnd));
		}

		// surf points to the next lod now.
		lod->ofsEnd = (int)((byte *)surf - (byte *)lod);
		lod = (mdrLOD_t *)surf;

		// find the next LOD.
		curlod = (mdrLOD_t *)((byte *)curlod + LittleLong(curlod->ofsEnd));
	}

	// lod points to the first tag now, so update the offset too.
	tag = (mdrTag_t *)lod;
	mdr->ofsTags = (int)((byte *)tag - (byte *)mdr);
	curtag = (mdrTag_t *)((byte *)pinmodel + LittleLong(pinmodel->ofsTags));

	// simple bounds check
	if (mdr->numTags < 0 || (byte *)(tag + mdr->numTags) >(byte *) mdr + size)
	{
		common->Warning("R_LoadMDR: %s has broken structure.\n", mod_name);
		return false;
	}

	for (i = 0; i < mdr->numTags; i++)
	{
		tag->boneIndex = LittleLong(curtag->boneIndex);
		strcpy(tag->name, curtag->name);

		tag++;
		curtag++;
	}

	// And finally we know the real offset to the end.
	mdr->ofsEnd = (int)((byte *)tag - (byte *)mdr);

	// Store the pointers for each frame, so we have it for faster lookup.
	for(int frameId = 0; frameId < mdr->numFrames; frameId++)
	{
		mdrHeader_t *header;
		mdrFrame_t *frame;
	
		header = (mdrHeader_t *)modelData;
		int frameSize = (size_t)(&((mdrFrame_t *)0)->bones[header->numBones]);
	
		frame = (mdrFrame_t *)((byte *)header + header->ofsFrames + frameId * frameSize);
		frames.Append(frame);
	}

	return true;
}

/*
=================
idRenderModelMD3::IsDynamicModel
=================
*/
dynamicModel_t rvmRenderModelMDR::IsDynamicModel() const {
	//if (modelData->numFrames == 1) {
	//	return DM_STATIC;
	//}
	//return DM_CACHED;
	return DM_STATIC;
}

/*
=================
idRenderModelMD3::TouchData
=================
*/
void rvmRenderModelMDR::TouchData() {

}

/*
=================
idRenderModelMD3::TouchData
=================
*/
int	rvmRenderModelMDR::NumJoints(void) const {
	return modelData->numBones;
}

/*
=================
idRenderModelMD3::GetJoints
=================
*/
const idMD5Joint *rvmRenderModelMDR::GetJoints(void) const {
	return nullptr;
}

/*
=================
idRenderModelMD3::GetJointHandle
=================
*/
jointHandle_t rvmRenderModelMDR::GetJointHandle(const char *name) const {
	return (jointHandle_t)-1;
}

/*
=================
idRenderModelMD3::Bounds
=================
*/
idBounds rvmRenderModelMDR::Bounds(const struct renderEntity_t *ent) const {
	return bounds;
}

/*
=================
idRenderModelMD3::Memory
=================
*/
int	rvmRenderModelMDR::Memory() const {
	return dataSize;
}

/*
=================
rvmRenderModelMDR::RenderFramesToModel
=================
*/
void rvmRenderModelMDR::RenderFramesToModel(idRenderModelStatic *staticModel, const struct renderEntity_s *ent, const struct viewDef_s *view, const idMaterial *overrideMaterial, int scale) {
	int				i, j, k;
	float			frontlerp, backlerp;
	int				*triangles;
	int				indexes;
	int				baseIndex, baseVertex;
	int				numVerts;
	mdrVertex_t		*v;
	mdrHeader_t		*header;
	mdrFrame_t		*frame;
	mdrFrame_t		*oldFrame;
	mdrBone_t		bones[MDR_MAX_BONES], *bonePtr, *bone;

	int			frameSize;

	int currentFrame = 0;
	int oldFrameNum = 0;

	// don't lerp if lerping off, or this is the only frame, or the last frame...
	//
	//if (backEnd.currentEntity->e.oldframe == backEnd.currentEntity->e.frame)
	//{
		backlerp = 0;	// if backlerp is 0, lerping is off and frontlerp is never used
		frontlerp = 1;
	//}
	//else
	//{
	//	backlerp = backEnd.currentEntity->e.backlerp;
	//	frontlerp = 1.0f - backlerp;
	//}

	header = modelData;

	frameSize = (size_t)(&((mdrFrame_t *)0)->bones[header->numBones]);

	frame = (mdrFrame_t *)((byte *)header + header->ofsFrames + currentFrame * frameSize);
	oldFrame = (mdrFrame_t *)((byte *)header + header->ofsFrames + oldFrameNum * frameSize);

	
	mdrLOD_t *lod = (mdrLOD_t *)((byte *)header + header->ofsLODs);
	mdrSurface_t *surface = (mdrSurface_t *)((byte *)lod + lod->ofsSurfaces);

	baseIndex = 0;
	baseVertex = 0;
	for (int i = 0; i < lod->numSurfaces; i++)
	{
		srfTriangles_t *tri = R_AllocStaticTriSurf();
		R_AllocStaticTriSurfVerts(tri, surface->numVerts);
		R_AllocStaticTriSurfIndexes(tri, surface->numTriangles * 3);
		tri->bounds.Clear();

		modelSurface_t	surf;

		surf.id = i;
		surf.geometry = tri;

		surf.shader = materials[i];

		// Set up all triangles.
		triangles = (int *)((byte *)surface + surface->ofsTriangles);
		indexes = surface->numTriangles * 3;
		for (j = 0; j < indexes; j++)
		{
			tri->indexes[baseIndex + j] = baseVertex + triangles[j];
		}
		tri->numIndexes += indexes;

		//
		// lerp all the needed bones
		//
		if (!backlerp)
		{
			// no lerping needed
			bonePtr = frame->bones;
		}
		else
		{
			bonePtr = bones;

			for (i = 0; i < header->numBones * 12; i++)
			{
				((float *)bonePtr)[i] = frontlerp * ((float *)frame->bones)[i] + backlerp * ((float *)oldFrame->bones)[i];
			}
		}

		//
		// deform the vertexes by the lerped bones
		//
		numVerts = surface->numVerts;
		v = (mdrVertex_t *)((byte *)surface + surface->ofsVerts);
		for (j = 0; j < numVerts; j++)
		{
			idVec3	tempVert, tempNormal;
			mdrWeight_t	*w;

			tempVert.Zero();
			tempNormal.Zero();
			w = v->weights;
			for (k = 0; k < v->numWeights; k++, w++)
			{
				bone = bonePtr + w->boneIndex;

				tempVert[0] += w->boneWeight * (DotProduct(bone->matrix[0], w->offset) + bone->matrix[0][3]);
				tempVert[1] += w->boneWeight * (DotProduct(bone->matrix[1], w->offset) + bone->matrix[1][3]);
				tempVert[2] += w->boneWeight * (DotProduct(bone->matrix[2], w->offset) + bone->matrix[2][3]);

				tempNormal[0] += w->boneWeight * DotProduct(bone->matrix[0], v->normal);
				tempNormal[1] += w->boneWeight * DotProduct(bone->matrix[1], v->normal);
				tempNormal[2] += w->boneWeight * DotProduct(bone->matrix[2], v->normal);
			}

			tri->verts[baseVertex + j].xyz[0] = tempVert[0];
			tri->verts[baseVertex + j].xyz[1] = tempVert[1];
			tri->verts[baseVertex + j].xyz[2] = tempVert[2];

		//	R_VaoPackNormal(tess.normal[baseVertex + j], tempNormal);

			tri->verts[baseVertex + j].st[0] = v->texCoords[0];
			tri->verts[baseVertex + j].st[1] = v->texCoords[1];

			v = (mdrVertex_t *)&v->weights[v->numWeights];
		}

		//tess.numVertexes += surface->numVerts;
		tri->numVerts += surface->numVerts;

		//baseVertex += surface->numVerts;
		//baseIndex += surface->numTriangles * 3;

		R_BoundTriSurf(tri);

		staticModel->AddSurface(surf);
		staticModel->bounds.AddPoint(surf.geometry->bounds[0]);
		staticModel->bounds.AddPoint(surf.geometry->bounds[1]);

		surface = (mdrSurface_t *)((byte *)surface + surface->ofsEnd);
	}
}

/*
=================
rvmRenderModelMDR::InstantiateDynamicModel
=================
*/
idRenderModel *rvmRenderModelMDR::InstantiateDynamicModel(const struct renderEntity_t *ent, const struct viewDef_s *view, idRenderModel *cachedModel) {
	idRenderModelStatic	*staticModel;

	if (cachedModel) {
		delete cachedModel;
		cachedModel = NULL;
	}

	staticModel = new idRenderModelStatic;
	staticModel->bounds.Clear();

	int scale = 1; // ent->scale;
	if (scale == 0)
		scale = 1;

	if (ent != NULL)
	{
		RenderFramesToModel(staticModel, (const renderEntity_s*)ent, view, ent->customShader, scale);
	}
	else
	{
		RenderFramesToModel(staticModel, NULL, view, NULL, scale);
	}

	return staticModel;
}


/*
=================
idRenderModelMD3::GetJointHandle
=================
*/
const char *		rvmRenderModelMDR::GetJointName(jointHandle_t handle) const {
	return "not_implemented";
}

/*
=================
idRenderModelMD3::GetDefaultPose
=================
*/
const idJointQuat *	rvmRenderModelMDR::GetDefaultPose(void) const {
	return nullptr;
}
/*
=================
idRenderModelMD3::NearestJoint
=================
*/
int	rvmRenderModelMDR::NearestJoint(int surfaceNum, int a, int b, int c) const {
	return 0;
}