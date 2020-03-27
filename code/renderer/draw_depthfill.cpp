// draw_depthfill.cpp
//

#include "engine_precompiled.h"
#include "tr_local.h"

/*
==================
RB_T_FillDepthBuffer
==================
*/
void RB_T_FillDepthBuffer(const drawSurf_t* surf) {
	int			stage;
	const idMaterial* shader;
	const shaderStage_t* pStage;
	const float* regs;
	float		color[4];
	const srfTriangles_t* tri;

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

	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);

	idDrawVert* ac = (idDrawVert*)vertexCache.Position(tri->ambientCache);
	glVertexPointer(3, GL_FLOAT, sizeof(idDrawVert), ac->xyz.ToFloatPtr());
	glTexCoordPointer(2, GL_FLOAT, sizeof(idDrawVert), reinterpret_cast<void*>(&ac->st));
	glVertexAttribPointerARB(PC_ATTRIB_INDEX_NORMAL, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->normal.ToFloatPtr());

	bool drawSolid = false;

	if (shader->Coverage() == MC_OPAQUE) {
		drawSolid = true;
	}

	if (surf->skinning.HasSkinning()) {
		renderProgManager.BindShader_DepthSkinned();
	}
	else {
		renderProgManager.BindShader_Depth();
	}

	// we may have multiple alpha tested stages
	if (shader->Coverage() == MC_PERFORATED) {
		// if the only alpha tested stages are condition register omitted,
		// draw a normal opaque surface
		bool	didDraw = false;
	
		glEnable(GL_ALPHA_TEST);
		// perforated surfaces may have multiple alpha tested stages
		for (stage = 0; stage < shader->GetNumStages(); stage++) {
			pStage = shader->GetStage(stage);
	
			if (!pStage->hasAlphaTest) {
				continue;
			}
	
			// check the stage enable condition
			if (regs[pStage->conditionRegister] == 0) {
				continue;
			}
	
			// if we at least tried to draw an alpha tested stage,
			// we won't draw the opaque surface
			didDraw = true;
	
			// set the alpha modulate
			color[3] = regs[pStage->color.registers[3]];
	
			// skip the entire stage if alpha would be black
			if (color[3] <= 0) {
				continue;
			}
			glColor4fv(color);
	
			glAlphaFunc(GL_GREATER, regs[pStage->alphaTestRegister]);
	
			// bind the texture
			pStage->texture.image->Bind();
	
			// set texture matrix and texGens
			RB_PrepareStageTexturing(pStage, surf, ac);
	
			// draw it
			RB_DrawElementsWithCounters(tri);
	
			RB_FinishStageTexturing(pStage, surf, ac);
		}
		glDisable(GL_ALPHA_TEST);
		if (!didDraw) {
			drawSolid = true;
		}
	}

	if (surf->skinning.HasSkinning()) {
		const rvmSkeletalSurf_t* skinning = &surf->skinning;
		RB_BindJointBuffer(skinning->jointBuffer, skinning->jointsInverted->ToFloatPtr(), skinning->numInvertedJoints, (void*)&ac->color, (void*)&ac->color2);
	}

	float modelViewMatrixTranspose[16];
	R_MatrixTranspose(surf->space->modelViewMatrix, modelViewMatrixTranspose);
	RB_SetVertexParms(RENDERPARM_MODELVIEWMATRIX_X, modelViewMatrixTranspose, 4);

	//for (stage = 0; stage < shader->GetNumStages(); stage++) {
	//
	//}
	//
	//Draw_SetVertexParm(RENDERPARM_DIFFUSEMATRIX_S, din->diffuseMatrix[0].ToFloatPtr());
	//Draw_SetVertexParm(RENDERPARM_DIFFUSEMATRIX_T, din->diffuseMatrix[1].ToFloatPtr());

	idImage* emissiveImage = surf->material->GetEmissiveImage();
	if(emissiveImage == NULL) {
		emissiveImage = globalImages->blackImage;
	}

	GL_SelectTextureNoClient(1);
	emissiveImage->Bind();

	GL_SelectTextureNoClient(0);

	// draw the entire surface solid
	if (drawSolid) {
		glColor4fv(color);
		globalImages->whiteImage->Bind();

		// draw it
		RB_DrawElementsWithCounters(tri);
	}

	GL_SelectTextureNoClient(1);
	globalImages->BindNull();

	GL_SelectTextureNoClient(0);

	if (surf->skinning.HasSkinning()) {
		RB_UnBindJointBuffer();
	}

	// reset polygon offset
	if (shader->TestMaterialFlag(MF_POLYGONOFFSET)) {
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	// reset blending
	if (shader->GetSort() == SS_SUBVIEW) {
		GL_State(GLS_DEPTHFUNC_LESS);
	}

	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
}

/*
=====================
RB_STD_FillDepthBuffer

If we are rendering a subview with a near clip plane, use a second texture
to force the alpha test to fail when behind that clip plane
=====================
*/
void RB_STD_FillDepthBuffer(drawSurf_t** drawSurfs, int numDrawSurfs) {
	// if we are just doing 2D rendering, no need to fill the depth buffer
	if (!backEnd.viewDef->viewEntitys) {
		return;
	}

	RB_LogComment("---------- RB_STD_FillDepthBuffer ----------\n");

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

	// Enable stencil test if we are going to be using it for shadows.
	// If we didn't do this, it would be legal behavior to get z fighting
	// from the ambient pass and the light passes.
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 255);

	RB_RenderDrawSurfListWithFunction(drawSurfs, numDrawSurfs, RB_T_FillDepthBuffer);

	if (backEnd.viewDef->numClipPlanes) {
		GL_SelectTexture(1);
		globalImages->BindNull();
		glDisable(GL_TEXTURE_GEN_S);
		GL_SelectTexture(0);
	}

}