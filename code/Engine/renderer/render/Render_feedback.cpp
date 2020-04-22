// draw_feedback.cpp
//

#include "engine_precompiled.h"
#include "../tr_local.h"

renderClassWorldType_t currentClassDrawType;

/*
==================
RB_T_FillFeedbackPass
==================
*/
void RB_T_FillFeedbackPass(const drawSurf_t *surf) {
	int			stage;
	const idMaterial	*shader;
	const shaderStage_t *pStage;
	const float	*regs;
	float		color[4];
	const srfTriangles_t	*tri;
	const rvmSkeletalSurf_t* skinning = &surf->skinning;

	if(surf->skinning.HasSkinning()) {
		renderProgManager.BindShader_VirtualTextureFeedback_Skinning();
	}
	else {
		renderProgManager.BindShader_VirtualTextureFeedback();
	}

	if (surf->surfaceClassType != backEnd.currentClassDrawType) {
		return;
	}

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

	if(surf->skinning.HasSkinning()) {
		RB_BindJointBuffer(skinning->jointBuffer, skinning->jointsInverted->ToFloatPtr(), skinning->numInvertedJoints, (void*)&ac->color, (void*)&ac->color2);
	}

	idVec4 diffuseMatrix[2];
	idVec4 virtualTextureFeedbackParms;
	if (surf->material->IsVirtualTextured())
	{
		virtualTextureFeedbackParms.x = shader->GetVirtualTexturePageSource() + 1;
		virtualTextureFeedbackParms.y = shader->GetVirtualAlbedoImage()->GetWidth(0);
		virtualTextureFeedbackParms.z = shader->GetVirtualAlbedoImage()->GetHeight(0);
		if (surf->forceVirtualTextureHighQuality)
		{
			virtualTextureFeedbackParms.w = 0;
		}
		else
		{
			virtualTextureFeedbackParms.w = shader->GetVirtualAlbedoImage()->GetNumMips();
		}
		R_ExtractInteractionTextureMatrix(surf->material->GetAlbedoTextureStage(), surf->shaderRegisters, diffuseMatrix);
	}
	else
	{
		virtualTextureFeedbackParms.x = 0;
		virtualTextureFeedbackParms.y = 0;
		virtualTextureFeedbackParms.z = 0;
		virtualTextureFeedbackParms.w = 0;
		diffuseMatrix[0].Zero();
		diffuseMatrix[1].Zero();
	}

	Draw_SetVertexParm(RENDERPARM_DIFFUSEMATRIX_S, diffuseMatrix[0].ToFloatPtr());
	Draw_SetVertexParm(RENDERPARM_DIFFUSEMATRIX_T, diffuseMatrix[1].ToFloatPtr());
	RB_SetVertexParm(RENDERPARAM_VIRTUALTEXTUREFEEDBACK, virtualTextureFeedbackParms.ToFloatPtr());

	glColor4fv(color);
	globalImages->whiteImage->Bind();

	// texture 1 will be the virtual texture offset map
	GL_SelectTextureNoClient(0);
	if (surf->material->IsVirtualTextured())
	{
		surf->material->GetVirtualPageOffsetsImage()->Bind();
	}
	else
	{
		globalImages->blackImage->Bind();
	}

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

	if (surf->skinning.HasSkinning()) {
		RB_UnBindJointBuffer();
	}
}

/*
===========================
RB_STD_DrawFeedbackPass
===========================
*/
void RB_STD_DrawFeedbackPass(drawSurf_t	 **drawSurfs, int numDrawSurfs) {
	// if we are just doing 2D rendering, no need to fill the feedback pass
	if (!backEnd.viewDef->viewEntitys) {
		return;
	}

	rvmDeviceDebugMarker deviceDebugMarker("FeedBack");

	RB_LogComment("---------- RB_STD_DrawFeedbackPass ----------\n");

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

	Draw_SetVertexParm(RENDERPARM_GLOBALEYEPOS, backEnd.viewDef->renderView.vieworg.ToFloatPtr());

	GL_State(GLS_DEPTHFUNC_LESS);

	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);

	backEnd.ignoreScissorOptimization = true;
		
		// Render the sky portal geometry.
		backEnd.currentClassDrawType = RENDER_CLASS_SKYPORTAL;
		RB_RenderDrawSurfListWithFunction(drawSurfs, numDrawSurfs, RB_T_FillFeedbackPass);
		
		// Nuke the depth buffer.
		glClear(GL_DEPTH_BUFFER_BIT);

		// Render the world.
		backEnd.currentClassDrawType = RENDER_CLASS_WORLD;
		RB_RenderDrawSurfListWithFunction(drawSurfs, numDrawSurfs, RB_T_FillFeedbackPass);
	backEnd.ignoreScissorOptimization = false;

	renderProgManager.Unbind();

	if (backEnd.viewDef->numClipPlanes) {
		GL_SelectTexture(1);
		globalImages->BindNull();
		glDisable(GL_TEXTURE_GEN_S);
		GL_SelectTexture(0);
	}
}