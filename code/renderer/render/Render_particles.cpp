// draw_particles.cpp
//

#include "engine_precompiled.h"
#include "../tr_local.h"

/*
==================
RB_STD_RenderParticleSurface

This is also called for the generated 2D rendering
==================
*/
void RB_STD_RenderParticleSurface(const drawSurf_t* surf) {
	int			stage;
	const idMaterial* shader;
	const shaderStage_t* pStage;
	const float* regs;
	float		color[4];
	const srfTriangles_t* tri;

	tri = surf->geo;
	shader = surf->material;

	if (!shader->HasAmbient()) {
		return;
	}

	if (shader->IsPortalSky()) {
		return;
	}

	// change the matrix if needed
	if (surf->space != backEnd.currentSpace) {
		glLoadMatrixf(surf->space->modelViewMatrix);
		backEnd.currentSpace = surf->space;
		RB_SetProgramEnvironmentSpace();
	}

	// change the scissor if needed
	if (r_useScissor.GetBool() && !backEnd.currentScissor.Equals(surf->scissorRect)) {
		backEnd.currentScissor = surf->scissorRect;
		glScissor(backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1,
			backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
			backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
			backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1);
	}

	// some deforms may disable themselves by setting numIndexes = 0
	if (!tri->numIndexes) {
		return;
	}

	if (!tri->ambientCache) {
		common->Printf("RB_T_RenderShaderPasses: !tri->ambientCache\n");
		return;
	}

	// get the expressions for conditionals / color / texcoords
	regs = surf->shaderRegisters;

	// set face culling appropriately
	GL_Cull(shader->GetCullType());

	// set polygon offset if necessary
	if (shader->TestMaterialFlag(MF_POLYGONOFFSET)) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * shader->GetPolygonOffset());
	}

	if (surf->space->weaponDepthHack) {
		RB_EnterWeaponDepthHack();
	}

	if (surf->space->modelDepthHack != 0.0f) {
		RB_EnterModelDepthHack(surf->space->modelDepthHack);
	}

	idDrawVert* ac = (idDrawVert*)vertexCache.Position(tri->ambientCache);
	glVertexPointer(3, GL_FLOAT, sizeof(idDrawVert), ac->xyz.ToFloatPtr());
	glTexCoordPointer(2, GL_FLOAT, sizeof(idDrawVert), reinterpret_cast<void*>(&ac->st));

	for (stage = 0; stage < shader->GetNumStages(); stage++) {
		pStage = shader->GetStage(stage);

		// check the enable condition
		if (regs[pStage->conditionRegister] == 0) {
			continue;
		}

		// skip the stages involved in lighting
		if (pStage->lighting != SL_AMBIENT) {
			continue;
		}

		// skip if the stage is ( GL_ZERO, GL_ONE ), which is used for some alpha masks
		if ((pStage->drawStateBits & (GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS)) == (GLS_SRCBLEND_ZERO | GLS_DSTBLEND_ONE)) {
			continue;
		}

		// completely skip the stage if we don't have the capability
		if (tr.backEndRenderer != BE_ARB2) {
			continue;
		}
		if (r_skipNewAmbient.GetBool()) {
			continue;
		}

		glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
		glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
		glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
		glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
		glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);

		//glVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
		//glVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
		//glNormalPointer( GL_FLOAT, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(idDrawVert), (void *)&ac->color);
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_NORMAL, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->normal.ToFloatPtr());
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_TANGENT, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->tangents[0].ToFloatPtr());
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_ST, 2, GL_FLOAT, false, sizeof(idDrawVert), ac->st.ToFloatPtr());
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_VERTEX, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->xyz.ToFloatPtr());

		GL_State(pStage->drawStateBits);

		RB_SetMVP(surf->space->mvp);

		//renderProgManager.BindShader(newStage->glslProgram, newStage->glslProgram);
		if(shader->ForceBloom()) {
			renderProgManager.BindShader_ParticleBloom();
		}
		else {
			renderProgManager.BindShader_Particle();
		}

		GL_SelectTexture(0);
		pStage->texture.image->Bind();

		// draw it
		RB_DrawElementsWithCounters(tri);

		GL_SelectTexture(0);

		// Fixme: Hack to get around an apparent bug in ATI drivers.  Should remove as soon as it gets fixed.
		renderProgManager.Unbind();

		glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
		glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
		glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
		glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
		glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
	}
}

/*
=====================
RB_STD_DrawParticles

Draw non-light dependent passes
=====================
*/
int RB_STD_DrawParticles(drawSurf_t** drawSurfs, int numDrawSurfs) {
	int				i;

	RB_LogComment("---------- RB_STD_DrawParticles ----------\n");

	GL_SelectTexture(1);
	globalImages->BindNull();

	GL_SelectTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	RB_SetProgramEnvironment();

	// we don't use RB_RenderDrawSurfListWithFunction()
	// because we want to defer the matrix load because many
	// surfaces won't draw any ambient passes
	backEnd.currentSpace = NULL;
	for (i = 0; i < numDrawSurfs; i++) {
		if (drawSurfs[i]->material->SuppressInSubview()) {
			continue;
		}

		if (backEnd.viewDef->isXraySubview && drawSurfs[i]->space->entityDef) {
			if (drawSurfs[i]->space->entityDef->parms.xrayIndex != 2) {
				continue;
			}
		}

		// we need to draw the post process shaders after we have drawn the fog lights
		if (drawSurfs[i]->material->GetSort() >= SS_POST_PROCESS
			&& !backEnd.currentRenderCopied) {
			break;
		}

		if (!drawSurfs[i]->material->IsParticle())
			continue;

		RB_STD_RenderParticleSurface(drawSurfs[i]);
	}

	GL_Cull(CT_FRONT_SIDED);
	glColor3f(1, 1, 1);

	return i;
}

