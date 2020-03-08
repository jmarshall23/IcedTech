// draw_shadow.cpp
//

#include "engine_precompiled.h"
#include "tr_local.h"

idCVar r_shadowMapAtlasSize("r_shadowMapAtlasSize", "8192", CVAR_INTEGER | CVAR_ROM, "The size of the shadow map atlas size to use");
idCVar r_shadowMapAtlasSliceSize("r_shadowMapAtlasSliceSize", "512", CVAR_INTEGER | CVAR_ROM, "The size of the shadow map slice in the atlas");
/*
=========================
R_GetShadowMapAtlasSlice
=========================
*/
ID_INLINE rvmShadowMapAtlasSlice_t *R_GetShadowMapAtlasSlice(int x, int y) {
	int numSlices = r_shadowMapAtlasSize.GetInteger() / r_shadowMapAtlasSliceSize.GetInteger();
	return &tr.shadowMapAtlasLookup[(y * numSlices) + x];
}

/*
=========================
R_InitShadowMapSystem
=========================
*/
void R_InitShadowMapSystem(void) {
	return;

	idImageOpts opts;
	opts.colorFormat = CFM_DEFAULT;
	opts.numLevels = 1;
	opts.textureType = TT_2D;
	opts.isPersistant = true;
	opts.width = r_shadowMapAtlasSize.GetInteger();
	opts.height = r_shadowMapAtlasSize.GetInteger();
	opts.readback = 1;
	opts.numMSAASamples = 0;
	opts.format = FMT_DEPTH;

	tr.shadowMapAtlasImage = globalImages->ScratchImage("_shadowMapAtlas", &opts, TF_LINEAR, TR_CLAMP, TD_DEFAULT);
	if(!tr.shadowMapAtlasImage->IsLoaded())
	{
		tr.shadowMapAtlasImage->AllocImage(opts, TF_DEFAULT, TR_REPEAT);
	}

	tr.shadowMapAtlas = renderSystem->CreateRenderTexture(nullptr, tr.shadowMapAtlasImage);

	int numSlices = r_shadowMapAtlasSize.GetInteger() / r_shadowMapAtlasSliceSize.GetInteger();

	tr.shadowMapAtlasLookup = new rvmShadowMapAtlasSlice_t[numSlices * numSlices];	

	// Create the shadow atlas slice lookup table.
	for(int y = 0; y < numSlices; y++)
	{
		for(int x = 0; x < numSlices; x++)
		{
			rvmShadowMapAtlasSlice_t *slice = R_GetShadowMapAtlasSlice(x, y);
			slice->x = x * r_shadowMapAtlasSliceSize.GetInteger();
			slice->y = y * r_shadowMapAtlasSliceSize.GetInteger();
		}
	}
}

/*
==================
RB_T_FillShadowMapOccluder
==================
*/
void RB_T_FillShadowMapOccluder(const drawSurf_t *surf, float side, float _near, float radius) {
	int			stage;
	const idMaterial	*shader;
	const shaderStage_t *pStage;
	const float	*regs;
	float		color[4];
	const srfTriangles_t	*tri;

	tri = surf->geo;
	shader = surf->material;

	// update the clip plane if needed
	if (backEnd.viewDef->numClipPlanes && surf->space != backEnd.currentSpace) {
		GL_SelectTexture(1);

		idPlane	plane;

		R_GlobalPlaneToLocal(surf->space->modelMatrix, backEnd.viewDef->clipPlanes[0], plane);
		plane[3] += 0.5;	// the notch is in the middle
		glTexGenfv(GL_S, GL_OBJECT_PLANE, plane.ToFloatPtr());
		GL_SelectTexture(0);
	}

	if (!shader->IsDrawn()) {
		return;
	}

	// some deforms may disable themselves by setting numIndexes = 0
	if (!tri->numIndexes) {
		return;
	}

	// translucent surfaces don't put anything in the depth buffer and don't
	// test against it, which makes them fail the mirror clip plane operation
	if (shader->Coverage() == MC_TRANSLUCENT) {
		return;
	}

	if (!tri->ambientCache) {
		common->Printf("RB_T_FillDepthBuffer: !tri->ambientCache\n");
		return;
	}

	// get the expressions for conditionals / color / texcoords
	regs = surf->shaderRegisters;

	// if all stages of a material have been conditioned off, don't do anything
	for (stage = 0; stage < shader->GetNumStages(); stage++) {
		pStage = shader->GetStage(stage);
		// check the stage enable condition
		if (regs[pStage->conditionRegister] != 0) {
			break;
		}
	}
	if (stage == shader->GetNumStages()) {
		return;
	}

	// set polygon offset if necessary
	if (shader->TestMaterialFlag(MF_POLYGONOFFSET)) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * shader->GetPolygonOffset());
	}

	// subviews will just down-modulate the color buffer by overbright
	if (shader->GetSort() == SS_SUBVIEW) {
		GL_State(GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ZERO | GLS_DEPTHFUNC_LESS);
		color[0] =
			color[1] =
			color[2] = (1.0 / backEnd.overBright);
		color[3] = 1;
	}
	else {
		// others just draw black
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		color[3] = 1;
	}

	idDrawVert *ac = (idDrawVert *)vertexCache.Position(tri->ambientCache);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);

	glVertexAttribPointerARB(PC_ATTRIB_INDEX_NORMAL, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->normal.ToFloatPtr());
	glVertexAttribPointerARB(PC_ATTRIB_INDEX_TANGENT, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->tangents[0].ToFloatPtr());
	glVertexAttribPointerARB(PC_ATTRIB_INDEX_ST, 2, GL_FLOAT, false, sizeof(idDrawVert), ac->st.ToFloatPtr());
	glVertexAttribPointerARB(PC_ATTRIB_INDEX_VERTEX, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->xyz.ToFloatPtr());


	idVec4 shadowMapInfo = idVec4(side, _near, radius, 0);
	Draw_SetVertexParm(RENDERPARM_SHADOWMAPINFO, shadowMapInfo.ToFloatPtr());

	// draw it
	RB_DrawElementsWithCounters(tri);


	// reset polygon offset
	if (shader->TestMaterialFlag(MF_POLYGONOFFSET)) {
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	// reset blending
	if (shader->GetSort() == SS_SUBVIEW) {
		GL_State(GLS_DEPTHFUNC_LESS);
	}

	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
}


/*
======================
RB_RenderDrawShadowSurfChainWithFunction
======================
*/
void RB_RenderDrawShadowSurfChainWithFunction(const drawSurf_t *drawSurfs, rvmShadowMapAtlasSlice_t &shadowMapAtlasSlice, float side, float _near, float radius, idRenderMatrix &shadowMapMatrix, void(*triFunc_)(const drawSurf_t *, float side, float near, float _far)) {
	const drawSurf_t		*drawSurf;
	idScreenRect clearScreenRect;

	clearScreenRect.Clear();

	backEnd.currentSpace = NULL;
	backEnd.currentScissor = clearScreenRect;

	glViewport(shadowMapAtlasSlice.x, shadowMapAtlasSlice.y, shadowMapAtlasSlice.x + r_shadowMapAtlasSliceSize.GetInteger(), shadowMapAtlasSlice.y + r_shadowMapAtlasSliceSize.GetInteger());
	glScissor(shadowMapAtlasSlice.x, shadowMapAtlasSlice.y, shadowMapAtlasSlice.x + r_shadowMapAtlasSliceSize.GetInteger(), shadowMapAtlasSlice.y + r_shadowMapAtlasSliceSize.GetInteger());

	for (drawSurf = drawSurfs; drawSurf; drawSurf = drawSurf->nextOnLight) {
		idRenderMatrix mvp;
		idRenderMatrix surfModelMatrixTransposed;
		idRenderMatrix surfModelMatrix(drawSurf->space->modelMatrix[0], drawSurf->space->modelMatrix[1], drawSurf->space->modelMatrix[2], drawSurf->space->modelMatrix[3],
			drawSurf->space->modelMatrix[4], drawSurf->space->modelMatrix[5], drawSurf->space->modelMatrix[6], drawSurf->space->modelMatrix[7],
			drawSurf->space->modelMatrix[8], drawSurf->space->modelMatrix[9], drawSurf->space->modelMatrix[10], drawSurf->space->modelMatrix[11],
			drawSurf->space->modelMatrix[12], drawSurf->space->modelMatrix[13], drawSurf->space->modelMatrix[14], drawSurf->space->modelMatrix[15]);

		idRenderMatrix::Transpose(surfModelMatrix, surfModelMatrixTransposed);

		idRenderMatrix::Multiply(surfModelMatrix, shadowMapMatrix, mvp);

		// change the matrix if needed
		RB_SetMVP(shadowMapMatrix);

		// render it
		triFunc_(drawSurf, side, _near, radius);
	}
}

/*
===========================
RB_STD_ShadowMapPass
===========================
*/
void RB_STD_ShadowMapPass(const drawSurf_t *drawSurfs, idRenderMatrix &shadowMapMatrix, int passId, float radius, rvmShadowMapAtlasSlice_t &shadowMapAtlasSlice) {
	// if we are just doing 2D rendering, no need to fill the feedback pass
	if (!backEnd.viewDef->viewEntitys) {
		return;
	}

	RB_LogComment("---------- RB_STD_ShadowMapPass ----------\n");

	// enable the second texture for mirror plane clipping if needed
	if (backEnd.viewDef->numClipPlanes) {
		GL_SelectTexture(1);
		globalImages->alphaNotchImage->Bind();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_GEN_S);
		glTexCoord2f(1, 0.5);
	}

	// the first texture will be used for alpha tested surfaces
	GL_SelectTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// decal surfaces may enable polygon offset
	glPolygonOffset(r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat());

	GL_State(GLS_DEPTHFUNC_LESS);

	RB_RenderDrawShadowSurfChainWithFunction(drawSurfs, shadowMapAtlasSlice, passId, 0.03, radius, shadowMapMatrix, RB_T_FillShadowMapOccluder);

	if (backEnd.viewDef->numClipPlanes) {
		GL_SelectTexture(1);
		globalImages->BindNull();
		glDisable(GL_TEXTURE_GEN_S);
		GL_SelectTexture(0);
	}
}
/*
======================
RB_DrawPointlightShadow
======================
*/
void RB_DrawPointlightShadow(viewLight_t *viewLight) {
	idMat3 lightAxis;
	idRenderMatrix renderMatrix;

	lightAxis.Identity();

	idRenderMatrix::CreateViewMatrix(viewLight->lightDef->parms.origin, lightAxis, renderMatrix);

	float lightradius = 0.0f;
	lightradius = max(lightradius, viewLight->lightDef->parms.lightRadius.x);
	lightradius = max(lightradius, viewLight->lightDef->parms.lightRadius.y);
	lightradius = max(lightradius, viewLight->lightDef->parms.lightRadius.z);
	
	renderProgManager.BindShader_ShadowDualParaboloid();
	
	RB_STD_ShadowMapPass(viewLight->localInteractions, renderMatrix, -1, lightradius, tr.shadowMapAtlasLookup[backEnd.c_numShadowMapSlices]);
	RB_STD_ShadowMapPass(viewLight->globalInteractions, renderMatrix, -1, lightradius, tr.shadowMapAtlasLookup[backEnd.c_numShadowMapSlices]);
	backEnd.c_numShadowMapSlices++;

	RB_STD_ShadowMapPass(viewLight->localInteractions, renderMatrix, 1, lightradius, tr.shadowMapAtlasLookup[backEnd.c_numShadowMapSlices]);
	RB_STD_ShadowMapPass(viewLight->globalInteractions, renderMatrix, 1, lightradius, tr.shadowMapAtlasLookup[backEnd.c_numShadowMapSlices]);
	backEnd.c_numShadowMapSlices++;

	renderProgManager.Unbind();
}

/*
======================
RB_Draw_ShadowMaps
======================
*/
void RB_Draw_ShadowMaps(void) {
	viewLight_t		*vLight;

	return;

	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);

	// Bind our shadow map atlas.
	tr.shadowMapAtlas->MakeCurrent();

	// ensures that depth writes are enabled for the depth clear
	GL_State(GLS_DEFAULT);

	glViewport(0, 0, r_shadowMapAtlasSize.GetInteger(), r_shadowMapAtlasSize.GetInteger());
	glScissor(0, 0, r_shadowMapAtlasSize.GetInteger(), r_shadowMapAtlasSize.GetInteger());

	glClear(GL_DEPTH_BUFFER_BIT);

	backEnd.c_numShadowMapSlices = 0;

	for (vLight = backEnd.viewDef->viewLights; vLight; vLight = vLight->next) {
		// If this light isn't casting shadows, skip it.
		if (vLight->lightDef->parms.noShadows)
			continue;

		// Ambient light doesn't cast shadows.
		if (vLight->lightDef->parms.ambientLight)
			continue;

		// Render the pointlight shadows
		if(vLight->lightDef->parms.pointLight) {
			RB_DrawPointlightShadow(vLight);
		}
	}

	// Reset the viewports.
	glViewport(tr.viewportOffset[0] + backEnd.viewDef->viewport.x1,
		tr.viewportOffset[1] + backEnd.viewDef->viewport.y1,
		backEnd.viewDef->viewport.x2 + 1 - backEnd.viewDef->viewport.x1,
		backEnd.viewDef->viewport.y2 + 1 - backEnd.viewDef->viewport.y1);

	// the scissor may be smaller than the viewport for subviews
	glScissor(tr.viewportOffset[0] + backEnd.viewDef->viewport.x1 + backEnd.viewDef->scissor.x1,
		tr.viewportOffset[1] + backEnd.viewDef->viewport.y1 + backEnd.viewDef->scissor.y1,
		backEnd.viewDef->scissor.x2 + 1 - backEnd.viewDef->scissor.x1,
		backEnd.viewDef->scissor.y2 + 1 - backEnd.viewDef->scissor.y1);

	// Reset the rendertarget.	
	idRenderTexture::BindNull();
}