// draw_sky.cpp
//

#include "engine_precompiled.h"
#include "../tr_local.h"

/*
========================
RB_T_DrawSkySurface
========================
*/
void RB_T_DrawSkySurface(const srfTriangles_t* tri, const idMaterial* shader) {
	const shaderStage_t* pStage;
	const float* regs;
	float		color[4];
	
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
		common->Printf("RB_T_DrawSkySurface: !tri->ambientCache\n");
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

	idDrawVert* ac = (idDrawVert*)vertexCache.Position(tri->ambientCache);
	glVertexPointer(3, GL_FLOAT, sizeof(idDrawVert), ac->xyz.ToFloatPtr());
	glTexCoordPointer(2, GL_FLOAT, sizeof(idDrawVert), reinterpret_cast<void*>(&ac->st));

	bool drawSolid = false;

	if (shader->Coverage() == MC_OPAQUE) {
		drawSolid = true;
	}

	shader->GetStage(0)->texture.image->Bind();

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

}

/*
======================
RB_STD_DrawSky
======================
*/
void RB_STD_DrawSky(void) {
	idRenderModel* skyModel = backEnd.viewDef->renderView.skyModel;

	// if we are just doing 2D rendering, no need to fill the depth buffer
	if (!backEnd.viewDef->viewEntitys) {
		return;
	}

	if (skyModel == NULL || backEnd.viewDef->renderView.skyMaterial == NULL)
		return;

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

	GL_State(GLS_DEPTHFUNC_LESS | GLS_DEPTHMASK);

	// Enable stencil test if we are going to be using it for shadows.
	// If we didn't do this, it would be legal behavior to get z fighting
	// from the ambient pass and the light passes.
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 255);

	float viewRotationMatrix[24];
	memcpy(viewRotationMatrix, backEnd.viewDef->worldSpace.transposedModelViewMatrix, sizeof(float) * 16);
	
	// Remove the translation from the view rotation matrix.
	viewRotationMatrix[3] = 0;
	viewRotationMatrix[7] = 0;
	viewRotationMatrix[11] = 0;
	viewRotationMatrix[16] = 1;

	idRenderMatrix modelViewProjectionMatrix;
	idRenderMatrix::Multiply(backEnd.viewDef->projectionRenderMatrix, *(idRenderMatrix*)viewRotationMatrix, modelViewProjectionMatrix);

	RB_SetMVP(modelViewProjectionMatrix);

	//RB_RenderDrawSurfListWithFunction(drawSurfs, numDrawSurfs, RB_T_FillDepthBuffer);

	renderProgManager.BindShader_SkyShader();

	for (int i = 0; i < skyModel->NumSurfaces(); i++) {
		RB_T_DrawSkySurface(skyModel->Surface(i)->geometry, backEnd.viewDef->renderView.skyMaterial);
	}

	renderProgManager.Unbind();

	if (backEnd.viewDef->numClipPlanes) {
		GL_SelectTexture(1);
		globalImages->BindNull();
		glDisable(GL_TEXTURE_GEN_S);
		GL_SelectTexture(0);
	}
}