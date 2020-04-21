/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#include "engine_precompiled.h"
#pragma hdrstop

#include "../tr_local.h"

/*
=====================
RB_BakeTextureMatrixIntoTexgen
=====================
*/
void RB_BakeTextureMatrixIntoTexgen( idPlane lightProject[3], const float *textureMatrix ) {
	float	genMatrix[16];
	float	final[16];

	genMatrix[0] = lightProject[0][0];
	genMatrix[4] = lightProject[0][1];
	genMatrix[8] = lightProject[0][2];
	genMatrix[12] = lightProject[0][3];

	genMatrix[1] = lightProject[1][0];
	genMatrix[5] = lightProject[1][1];
	genMatrix[9] = lightProject[1][2];
	genMatrix[13] = lightProject[1][3];

	genMatrix[2] = 0;
	genMatrix[6] = 0;
	genMatrix[10] = 0;
	genMatrix[14] = 0;

	genMatrix[3] = lightProject[2][0];
	genMatrix[7] = lightProject[2][1];
	genMatrix[11] = lightProject[2][2];
	genMatrix[15] = lightProject[2][3];

	myGlMultMatrix( genMatrix, backEnd.lightTextureMatrix, final );

	lightProject[0][0] = final[0];
	lightProject[0][1] = final[4];
	lightProject[0][2] = final[8];
	lightProject[0][3] = final[12];

	lightProject[1][0] = final[1];
	lightProject[1][1] = final[5];
	lightProject[1][2] = final[9];
	lightProject[1][3] = final[13];
}

/*
================
RB_PrepareStageTexturing
================
*/
void RB_PrepareStageTexturing( const shaderStage_t *pStage,  const drawSurf_t *surf, idDrawVert *ac ) {
	// set privatePolygonOffset if necessary
	if ( pStage->privatePolygonOffset ) {
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * pStage->privatePolygonOffset );
	}

	// set the texture matrix if needed
	if ( pStage->texture.hasMatrix ) {
		RB_LoadShaderTextureMatrix( surf->shaderRegisters, &pStage->texture );
	}

	// texgens
	if ( pStage->texture.texgen == TG_DIFFUSE_CUBE ) {
		glTexCoordPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
	}
	if ( pStage->texture.texgen == TG_SKYBOX_CUBE || pStage->texture.texgen == TG_WOBBLESKY_CUBE ) {
		glTexCoordPointer( 3, GL_FLOAT, 0, vertexCache.Position( surf->dynamicTexCoords ) );
	}
	if ( pStage->texture.texgen == TG_SCREEN ) {
		glEnable( GL_TEXTURE_GEN_S );
		glEnable( GL_TEXTURE_GEN_T );
		glEnable( GL_TEXTURE_GEN_Q );

		float	mat[16], plane[4];
		myGlMultMatrix( surf->space->modelViewMatrix, backEnd.viewDef->projectionMatrix, mat );

		plane[0] = mat[0];
		plane[1] = mat[4];
		plane[2] = mat[8];
		plane[3] = mat[12];
		glTexGenfv( GL_S, GL_OBJECT_PLANE, plane );

		plane[0] = mat[1];
		plane[1] = mat[5];
		plane[2] = mat[9];
		plane[3] = mat[13];
		glTexGenfv( GL_T, GL_OBJECT_PLANE, plane );

		plane[0] = mat[3];
		plane[1] = mat[7];
		plane[2] = mat[11];
		plane[3] = mat[15];
		glTexGenfv( GL_Q, GL_OBJECT_PLANE, plane );
	}

	if ( pStage->texture.texgen == TG_SCREEN2 ) {
		glEnable( GL_TEXTURE_GEN_S );
		glEnable( GL_TEXTURE_GEN_T );
		glEnable( GL_TEXTURE_GEN_Q );

		float	mat[16], plane[4];
		myGlMultMatrix( surf->space->modelViewMatrix, backEnd.viewDef->projectionMatrix, mat );

		plane[0] = mat[0];
		plane[1] = mat[4];
		plane[2] = mat[8];
		plane[3] = mat[12];
		glTexGenfv( GL_S, GL_OBJECT_PLANE, plane );

		plane[0] = mat[1];
		plane[1] = mat[5];
		plane[2] = mat[9];
		plane[3] = mat[13];
		glTexGenfv( GL_T, GL_OBJECT_PLANE, plane );

		plane[0] = mat[3];
		plane[1] = mat[7];
		plane[2] = mat[11];
		plane[3] = mat[15];
		glTexGenfv( GL_Q, GL_OBJECT_PLANE, plane );
	}

	if ( pStage->texture.texgen == TG_GLASSWARP ) {
		if ( tr.backEndRenderer == BE_ARB2 /*|| tr.backEndRenderer == BE_NV30*/ ) {
			glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_GLASSWARP );
			glEnable( GL_FRAGMENT_PROGRAM_ARB );

			GL_SelectTexture( 2 );
			globalImages->scratchImage->Bind();

			GL_SelectTexture( 1 );
			globalImages->scratchImage2->Bind();

			glEnable( GL_TEXTURE_GEN_S );
			glEnable( GL_TEXTURE_GEN_T );
			glEnable( GL_TEXTURE_GEN_Q );

			float	mat[16], plane[4];
			myGlMultMatrix( surf->space->modelViewMatrix, backEnd.viewDef->projectionMatrix, mat );

			plane[0] = mat[0];
			plane[1] = mat[4];
			plane[2] = mat[8];
			plane[3] = mat[12];
			glTexGenfv( GL_S, GL_OBJECT_PLANE, plane );

			plane[0] = mat[1];
			plane[1] = mat[5];
			plane[2] = mat[9];
			plane[3] = mat[13];
			glTexGenfv( GL_T, GL_OBJECT_PLANE, plane );

			plane[0] = mat[3];
			plane[1] = mat[7];
			plane[2] = mat[11];
			plane[3] = mat[15];
			glTexGenfv( GL_Q, GL_OBJECT_PLANE, plane );

			GL_SelectTexture( 0 );
		}
	}
/*
	if ( pStage->texture.texgen == TG_REFLECT_CUBE ) {
		if ( tr.backEndRenderer == BE_ARB2 ) {
			// see if there is also a bump map specified
			const shaderStage_t *bumpStage = surf->material->GetBumpStage();
			if ( bumpStage ) {
				// per-pixel reflection mapping with bump mapping
				GL_SelectTexture( 1 );
				bumpStage->texture.image->Bind();
				GL_SelectTexture( 0 );

				glNormalPointer( GL_FLOAT, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
				glVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
				glVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );

				glEnableVertexAttribArrayARB( 9 );
				glEnableVertexAttribArrayARB( 10 );
				glEnableClientState( GL_NORMAL_ARRAY );

				// Program env 5, 6, 7, 8 have been set in RB_SetProgramEnvironmentSpace

				glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_BUMPY_ENVIRONMENT );
				glEnable( GL_FRAGMENT_PROGRAM_ARB );
				glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_BUMPY_ENVIRONMENT );
				glEnable( GL_VERTEX_PROGRAM_ARB );
			} else {
				// per-pixel reflection mapping without a normal map
				glNormalPointer( GL_FLOAT, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
				glEnableClientState( GL_NORMAL_ARRAY );

				glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_ENVIRONMENT );
				glEnable( GL_FRAGMENT_PROGRAM_ARB );
				glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_ENVIRONMENT );
				glEnable( GL_VERTEX_PROGRAM_ARB );
			}
		} else {
			glEnable( GL_TEXTURE_GEN_S );
			glEnable( GL_TEXTURE_GEN_T );
			glEnable( GL_TEXTURE_GEN_R );
			glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT );
			glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT );
			glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT );
			glEnableClientState( GL_NORMAL_ARRAY );
			glNormalPointer( GL_FLOAT, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );

			glMatrixMode( GL_TEXTURE );
			float	mat[16];

			R_TransposeGLMatrix( backEnd.viewDef->worldSpace.modelViewMatrix, mat );

			glLoadMatrixf( mat );
			glMatrixMode( GL_MODELVIEW );
		}
	}
*/
}

/*
================
RB_FinishStageTexturing
================
*/
void RB_FinishStageTexturing( const shaderStage_t *pStage, const drawSurf_t *surf, idDrawVert *ac ) {
	// unset privatePolygonOffset if necessary
	if ( pStage->privatePolygonOffset && !surf->material->TestMaterialFlag(MF_POLYGONOFFSET) ) {
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	if ( pStage->texture.texgen == TG_DIFFUSE_CUBE || pStage->texture.texgen == TG_SKYBOX_CUBE
		|| pStage->texture.texgen == TG_WOBBLESKY_CUBE ) {
		glTexCoordPointer( 2, GL_FLOAT, sizeof( idDrawVert ), (void *)&ac->st );
	}

	if ( pStage->texture.texgen == TG_SCREEN ) {
		glDisable( GL_TEXTURE_GEN_S );
		glDisable( GL_TEXTURE_GEN_T );
		glDisable( GL_TEXTURE_GEN_Q );
	}
	if ( pStage->texture.texgen == TG_SCREEN2 ) {
		glDisable( GL_TEXTURE_GEN_S );
		glDisable( GL_TEXTURE_GEN_T );
		glDisable( GL_TEXTURE_GEN_Q );
	}

	if ( pStage->texture.texgen == TG_GLASSWARP ) {
		if ( tr.backEndRenderer == BE_ARB2 /*|| tr.backEndRenderer == BE_NV30*/ ) {
			GL_SelectTexture( 2 );
			globalImages->BindNull();

			GL_SelectTexture( 1 );
			if ( pStage->texture.hasMatrix ) {
				RB_LoadShaderTextureMatrix( surf->shaderRegisters, &pStage->texture );
			}
			glDisable( GL_TEXTURE_GEN_S );
			glDisable( GL_TEXTURE_GEN_T );
			glDisable( GL_TEXTURE_GEN_Q );
			glDisable( GL_FRAGMENT_PROGRAM_ARB );
			globalImages->BindNull();
			GL_SelectTexture( 0 );
		}
	}

	if ( pStage->texture.texgen == TG_REFLECT_CUBE ) {
		if ( tr.backEndRenderer == BE_ARB2 ) {
			// see if there is also a bump map specified
			const shaderStage_t *bumpStage = surf->material->GetBumpStage();
			if ( bumpStage ) {
				// per-pixel reflection mapping with bump mapping
				GL_SelectTexture( 1 );
				globalImages->BindNull();
				GL_SelectTexture( 0 );

				glDisableVertexAttribArrayARB( 9 );
				glDisableVertexAttribArrayARB( 10 );
			} else {
				// per-pixel reflection mapping without bump mapping
			}

			glDisableClientState( GL_NORMAL_ARRAY );
			glDisable( GL_FRAGMENT_PROGRAM_ARB );
			glDisable( GL_VERTEX_PROGRAM_ARB );
			// Fixme: Hack to get around an apparent bug in ATI drivers.  Should remove as soon as it gets fixed.
			glBindProgramARB( GL_VERTEX_PROGRAM_ARB, 0 );
		} else {
			glDisable( GL_TEXTURE_GEN_S );
			glDisable( GL_TEXTURE_GEN_T );
			glDisable( GL_TEXTURE_GEN_R );
			glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			glDisableClientState( GL_NORMAL_ARRAY );

			glMatrixMode( GL_TEXTURE );
			glLoadIdentity();
			glMatrixMode( GL_MODELVIEW );
		}
	}

	if ( pStage->texture.hasMatrix ) {
		glMatrixMode( GL_TEXTURE );
		glLoadIdentity();
		glMatrixMode( GL_MODELVIEW );
	}
}

/*
=============================================================================================

SHADER PASSES

=============================================================================================
*/

/*
==================
RB_SetProgramEnvironment

Sets variables that can be used by all vertex programs
==================
*/
void RB_SetProgramEnvironment( void ) {
	float	parm[4];
	int		pot;

	if ( !glConfig.ARBVertexProgramAvailable ) {
		return;
	}

#if 0
	// screen power of two correction factor, one pixel in so we don't get a bilerp
	// of an uncopied pixel
	int	 w = backEnd.viewDef->viewport.x2 - backEnd.viewDef->viewport.x1 + 1;
	pot = globalImages->currentRenderImage->GetUploadWidth();
	if ( w == pot ) {
		parm[0] = 1.0;
	} else {
		parm[0] = (float)(w-1) / pot;
	}

	int	 h = backEnd.viewDef->viewport.y2 - backEnd.viewDef->viewport.y1 + 1;
	pot = globalImages->currentRenderImage->GetUploadHeight();
	if ( h == pot ) {
		parm[1] = 1.0;
	} else {
		parm[1] = (float)(h-1) / pot;
	}

	parm[2] = 0;
	parm[3] = 1;
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 0, parm );
#else
	// screen power of two correction factor, assuming the copy to _currentRender
	// also copied an extra row and column for the bilerp
	int	 w = backEnd.viewDef->viewport.x2 - backEnd.viewDef->viewport.x1 + 1;
	pot = globalImages->currentRenderImage->GetUploadWidth();
	parm[0] = (float)w / pot;

	int	 h = backEnd.viewDef->viewport.y2 - backEnd.viewDef->viewport.y1 + 1;
	pot = globalImages->currentRenderImage->GetUploadHeight();
	parm[1] = (float)h / pot;

	parm[2] = 0;
	parm[3] = 1;
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 0, parm );
#endif

	glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 0, parm );

	// window coord to 0.0 to 1.0 conversion
	parm[0] = 1.0 / w;
	parm[1] = 1.0 / h;
	parm[2] = 0;
	parm[3] = 1;
	glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 1, parm );

	//
	// set eye position in global space
	//
	parm[0] = backEnd.viewDef->renderView.vieworg[0];
	parm[1] = backEnd.viewDef->renderView.vieworg[1];
	parm[2] = backEnd.viewDef->renderView.vieworg[2];
	parm[3] = 1.0;
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 1, parm );


}

/*
==================
RB_SetProgramEnvironmentSpace

Sets variables related to the current space that can be used by all vertex programs
==================
*/
void RB_SetProgramEnvironmentSpace( void ) {
	if ( !glConfig.ARBVertexProgramAvailable ) {
		return;
	}

	const struct viewEntity_s *space = backEnd.currentSpace;
	float	parm[4];

	// set eye position in local space
	R_GlobalPointToLocal( space->modelMatrix, backEnd.viewDef->renderView.vieworg, *(idVec3 *)parm );
	parm[3] = 1.0;
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 5, parm );

	// we need the model matrix without it being combined with the view matrix
	// so we can transform local vectors to global coordinates
	parm[0] = space->modelMatrix[0];
	parm[1] = space->modelMatrix[4];
	parm[2] = space->modelMatrix[8];
	parm[3] = space->modelMatrix[12];
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 6, parm );
	parm[0] = space->modelMatrix[1];
	parm[1] = space->modelMatrix[5];
	parm[2] = space->modelMatrix[9];
	parm[3] = space->modelMatrix[13];
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 7, parm );
	parm[0] = space->modelMatrix[2];
	parm[1] = space->modelMatrix[6];
	parm[2] = space->modelMatrix[10];
	parm[3] = space->modelMatrix[14];
	glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 8, parm );
}

/*
==================
RB_STD_T_RenderShaderPasses

This is also called for the generated 2D rendering
==================
*/
void RB_STD_T_RenderShaderPasses( const drawSurf_t *surf ) {
	int			stage;
	const idMaterial	*shader;
	const shaderStage_t *pStage;
	const float	*regs;
	float		color[4];
	const srfTriangles_t	*tri;

	tri = surf->geo;
	shader = surf->material;

	if ( !shader->HasAmbient() ) {
		return;
	}

	if ( shader->IsPortalSky() ) {
		return;
	}

	if(shader->IsParticle()) {
		return;
	}

	if(surf->surfaceClassType != backEnd.currentClassDrawType) {
		return;
	}

	// change the matrix if needed	
	if ( surf->space != backEnd.currentSpace ) {
		glLoadMatrixf( surf->space->modelViewMatrix );
		backEnd.currentSpace = surf->space;
		RB_SetProgramEnvironmentSpace();
	}

	// change the scissor if needed
	if(backEnd.currentClassDrawType == RENDER_CLASS_SKYPORTAL)
	{
		glScissor(backEnd.viewDef->viewport.x1,
			backEnd.viewDef->viewport.y1,
			backEnd.viewDef->viewport.x2 + 1,
			backEnd.viewDef->viewport.y2 + 1);
	}
	else if ( r_useScissor.GetBool() && !backEnd.currentScissor.Equals( surf->scissorRect )) {
		backEnd.currentScissor = surf->scissorRect;
		glScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
			backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
			backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
			backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
	}

	// some deforms may disable themselves by setting numIndexes = 0
	if ( !tri->numIndexes ) {
		return;
	}

	if ( !tri->ambientCache ) {
		common->Printf( "RB_T_RenderShaderPasses: !tri->ambientCache\n" );
		return;
	}

	// get the expressions for conditionals / color / texcoords
	regs = surf->shaderRegisters;

	// set face culling appropriately
	GL_Cull( shader->GetCullType() );

	// set polygon offset if necessary
	if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) {
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * shader->GetPolygonOffset() );
	}
	
	if ( surf->space->weaponDepthHack ) {
		RB_EnterWeaponDepthHack();
	}

	if ( surf->space->modelDepthHack != 0.0f ) {
		RB_EnterModelDepthHack( surf->space->modelDepthHack );
	}

	idDrawVert *ac = (idDrawVert *)vertexCache.Position( tri->ambientCache );
	glVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( idDrawVert ), reinterpret_cast<void *>(&ac->st) );

	for ( stage = 0; stage < shader->GetNumStages() ; stage++ ) {		
		pStage = shader->GetStage(stage);

		// check the enable condition
		if ( regs[ pStage->conditionRegister ] == 0 ) {
			continue;
		}

		// skip the stages involved in lighting
		if ( pStage->lighting != SL_AMBIENT ) {
			continue;
		}

		// skip if the stage is ( GL_ZERO, GL_ONE ), which is used for some alpha masks
		if ( ( pStage->drawStateBits & (GLS_SRCBLEND_BITS|GLS_DSTBLEND_BITS) ) == ( GLS_SRCBLEND_ZERO | GLS_DSTBLEND_ONE ) ) {
			continue;
		}

		// see if we are a new-style stage
		newShaderStage_t *newStage = pStage->newStage;
		if ( newStage ) {
			//--------------------------
			//
			// new style stages
			//
			//--------------------------

			// completely skip the stage if we don't have the capability
			if ( tr.backEndRenderer != BE_ARB2 ) {
				continue;
			}
			if ( r_skipNewAmbient.GetBool() ) {
				continue;
			}

			glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
			glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
			glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
			glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);

			//glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), (void *)&ac->color );
			//glVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
			//glVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
			//glNormalPointer( GL_FLOAT, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
			glVertexAttribPointerARB(PC_ATTRIB_INDEX_NORMAL, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->normal.ToFloatPtr());
			glVertexAttribPointerARB(PC_ATTRIB_INDEX_TANGENT, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->tangents[0].ToFloatPtr());
			glVertexAttribPointerARB(PC_ATTRIB_INDEX_ST, 2, GL_FLOAT, false, sizeof(idDrawVert), ac->st.ToFloatPtr());
			glVertexAttribPointerARB(PC_ATTRIB_INDEX_VERTEX, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->xyz.ToFloatPtr());

			if (surf->surfaceClassType == RENDER_CLASS_SKYPORTAL) {
				GL_State(GLS_DEPTHFUNC_LESS);

				float viewRotationMatrix[24];
				memcpy(viewRotationMatrix, backEnd.viewDef->worldSpace.transposedModelViewMatrix, sizeof(float) * 16);

				// Remove the translation from the view rotation matrix.
				viewRotationMatrix[3] = 0;
				viewRotationMatrix[7] = 0;
				viewRotationMatrix[11] = 0;
				viewRotationMatrix[16] = 1;

				idRenderMatrix modelViewProjectionMatrix;
				idRenderMatrix::Multiply(backEnd.viewDef->projectionRenderMatrix, *(idRenderMatrix*)viewRotationMatrix, modelViewProjectionMatrix);

				idVec4 color(1, 1, 1, 1);
				idVec4 textureMatrix(1, 1, 1, 1);
				RB_SetVertexParm(RENDERPARM_COLOR, color.ToFloatPtr());
				RB_SetVertexParm(RENDERPARM_TEXTUREMATRIX_S, textureMatrix.ToFloatPtr());
				RB_SetVertexParm(RENDERPARM_TEXTUREMATRIX_T, textureMatrix.ToFloatPtr());
				RB_SetMVP(modelViewProjectionMatrix);
			}
			else
			{
				GL_State(pStage->drawStateBits);
				RB_SetMVP(surf->space->mvp);
			}
			
			renderProgManager.BindShader(newStage->glslProgram, newStage->glslProgram);

			// megaTextures bind a lot of images and set a lot of parameters
			if ( newStage->megaTexture ) {
				newStage->megaTexture->SetMappingForSurface( tri );
				idVec3	localViewer;
				R_GlobalPointToLocal( surf->space->modelMatrix, backEnd.viewDef->renderView.vieworg, localViewer );
				newStage->megaTexture->BindForViewOrigin( localViewer );
			}

			for ( int i = 0 ; i < newStage->numVertexParms ; i++ ) {
				float parm[4];
				parm[0] = regs[newStage->vertexParms[i][0]];
				parm[1] = regs[newStage->vertexParms[i][1]];
				parm[2] = regs[newStage->vertexParms[i][2]];
				parm[3] = regs[newStage->vertexParms[i][3]];
				Draw_SetVertexParm((renderParm_t)(RENDERPARM_USER + i), parm);
			}

			for ( int i = 0 ; i < newStage->numFragmentProgramImages ; i++ ) {
				if ( newStage->fragmentProgramImages[i] ) {
					GL_SelectTexture( i );
					newStage->fragmentProgramImages[i]->Bind();
				}
			}

			// draw it
			RB_DrawElementsWithCounters( tri );

			for ( int i = 1 ; i < newStage->numFragmentProgramImages ; i++ ) {
				if ( newStage->fragmentProgramImages[i] ) {
					GL_SelectTexture( i );
					globalImages->BindNull();
				}
			}
			if ( newStage->megaTexture ) {
				newStage->megaTexture->Unbind();
			}

			GL_SelectTexture( 0 );

			// Fixme: Hack to get around an apparent bug in ATI drivers.  Should remove as soon as it gets fixed.
			renderProgManager.Unbind();

			glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
			glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
			glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
			glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
			continue;
		}

		//--------------------------
		//
		// old style stages
		//
		//--------------------------

		// set the color
		color[0] = regs[ pStage->color.registers[0] ];
		color[1] = regs[ pStage->color.registers[1] ];
		color[2] = regs[ pStage->color.registers[2] ];
		color[3] = regs[ pStage->color.registers[3] ];

		// skip the entire stage if an add would be black
		if ( ( pStage->drawStateBits & (GLS_SRCBLEND_BITS|GLS_DSTBLEND_BITS) ) == ( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE ) 
			&& color[0] <= 0 && color[1] <= 0 && color[2] <= 0 ) {
			continue;
		}

		// skip the entire stage if a blend would be completely transparent
		if ( ( pStage->drawStateBits & (GLS_SRCBLEND_BITS|GLS_DSTBLEND_BITS) ) == ( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA )
			&& color[3] <= 0 ) {
			continue;
		}

		// select the vertex color source
		if ( pStage->vertexColor == SVC_IGNORE ) {
			glColor4fv( color );
		} else {
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), (void *)&ac->color );
			glEnableClientState( GL_COLOR_ARRAY );

			if ( pStage->vertexColor == SVC_INVERSE_MODULATE ) {
				GL_TexEnv( GL_COMBINE_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_ONE_MINUS_SRC_COLOR );
				glTexEnvi( GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1 );
			}

			// for vertex color and modulated color, we need to enable a second
			// texture stage
			if ( color[0] != 1 || color[1] != 1 || color[2] != 1 || color[3] != 1 ) {
				GL_SelectTexture( 1 );

				globalImages->whiteImage->Bind();
				GL_TexEnv( GL_COMBINE_ARB );

				glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color );

				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_CONSTANT_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR );
				glTexEnvi( GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1 );

				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_CONSTANT_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA );
				glTexEnvi( GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1 );

				GL_SelectTexture( 0 );
			}
		}

		// bind the texture
		RB_BindVariableStageImage( &pStage->texture, regs );

		// set the state
		GL_State( pStage->drawStateBits );
		
		RB_PrepareStageTexturing( pStage, surf, ac );

		// draw it
		RB_DrawElementsWithCounters( tri );

		RB_FinishStageTexturing( pStage, surf, ac );
		
		if ( pStage->vertexColor != SVC_IGNORE ) {
			glDisableClientState( GL_COLOR_ARRAY );

			GL_SelectTexture( 1 );
			GL_TexEnv( GL_MODULATE );
			globalImages->BindNull();
			GL_SelectTexture( 0 );
			GL_TexEnv( GL_MODULATE );
		}
	}

	// reset polygon offset
	if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) {
		glDisable( GL_POLYGON_OFFSET_FILL );
	}
	if ( surf->space->weaponDepthHack || surf->space->modelDepthHack != 0.0f ) {
		RB_LeaveDepthHack();
	}
}

/*
=====================
RB_STD_DrawShaderPasses

Draw non-light dependent passes
=====================
*/
int RB_STD_DrawShaderPasses( drawSurf_t **drawSurfs, int numDrawSurfs ) {
	int				i;

	// only obey skipAmbient if we are rendering a view
	if ( backEnd.viewDef->viewEntitys && r_skipAmbient.GetBool() ) {
		return numDrawSurfs;
	}

	RB_LogComment( "---------- RB_STD_DrawShaderPasses ----------\n" );

	// if we are about to draw the first surface that needs
	// the rendering in a texture, copy it over
	if ( drawSurfs[0]->material->GetSort() >= SS_POST_PROCESS ) {
		if ( r_skipPostProcess.GetBool() ) {
			return 0;
		}

		// only dump if in a 3d view
		//if ( backEnd.viewDef->viewEntitys && tr.backEndRenderer == BE_ARB2 ) {
		//	globalImages->currentRenderImage->CopyFramebuffer( backEnd.viewDef->viewport.x1,
		//		backEnd.viewDef->viewport.y1,  backEnd.viewDef->viewport.x2 -  backEnd.viewDef->viewport.x1 + 1,
		//		backEnd.viewDef->viewport.y2 -  backEnd.viewDef->viewport.y1 + 1);
		//}
		//backEnd.currentRenderCopied = true;
	}

	GL_SelectTexture( 1 );
	globalImages->BindNull();

	GL_SelectTexture( 0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	RB_SetProgramEnvironment();

	// we don't use RB_RenderDrawSurfListWithFunction()
	// because we want to defer the matrix load because many
	// surfaces won't draw any ambient passes
	backEnd.currentSpace = NULL;
	for (i = 0  ; i < numDrawSurfs ; i++ ) {
		if ( drawSurfs[i]->material->SuppressInSubview() ) {
			continue;
		}

		if ( backEnd.viewDef->isXraySubview && drawSurfs[i]->space->entityDef ) {
			if ( drawSurfs[i]->space->entityDef->parms.xrayIndex != 2 ) {
				continue;
			}
		}

		// we need to draw the post process shaders after we have drawn the fog lights
		if ( drawSurfs[i]->material->GetSort() >= SS_POST_PROCESS
			&& !backEnd.currentRenderCopied ) {
			break;
		}

		RB_STD_T_RenderShaderPasses( drawSurfs[i] );
	}

	GL_Cull( CT_FRONT_SIDED );
	glColor3f( 1, 1, 1 );

	return i;
}



/*
==============================================================================

BACK END RENDERING OF STENCIL SHADOWS

==============================================================================
*/

/*
=====================
RB_T_Shadow

the shadow volumes face INSIDE
=====================
*/
static void RB_T_Shadow( const drawSurf_t *surf ) {
	const srfTriangles_t	*tri;
	idVec4 localLight;

	renderProgManager.BindShader_Shadow();

	// set the light position if we are using a vertex program to project the rear surfaces
	R_GlobalPointToLocal(surf->space->modelMatrix, backEnd.vLight->globalLightOrigin, localLight.ToVec3());
	localLight.w = 0.0f;
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_ORIGIN, localLight.ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_LOCALLIGHTORIGIN, localLight.ToFloatPtr());

	tri = surf->geo;

	if ( !tri->shadowCache ) {
		return;
	}

	glVertexPointer( 4, GL_FLOAT, sizeof( shadowCache_t ), vertexCache.Position(tri->shadowCache) );

	// we always draw the sil planes, but we may not need to draw the front or rear caps
	int	numIndexes;
	bool external = false;

	if ( !r_useExternalShadows.GetInteger() ) {
		numIndexes = tri->numIndexes;
	} else if ( r_useExternalShadows.GetInteger() == 2 ) { // force to no caps for testing
		numIndexes = tri->numShadowIndexesNoCaps;
	} else if ( !(surf->dsFlags & DSF_VIEW_INSIDE_SHADOW) ) { 
		// if we aren't inside the shadow projection, no caps are ever needed needed
		numIndexes = tri->numShadowIndexesNoCaps;
		external = true;
	} else if ( !backEnd.vLight->viewInsideLight && !(surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE) ) {
		// if we are inside the shadow projection, but outside the light, and drawing
		// a non-infinite shadow, we can skip some caps
		if ( backEnd.vLight->viewSeesShadowPlaneBits & surf->geo->shadowCapPlaneBits ) {
			// we can see through a rear cap, so we need to draw it, but we can skip the
			// caps on the actual surface
			numIndexes = tri->numShadowIndexesNoFrontCaps;
		} else {
			// we don't need to draw any caps
			numIndexes = tri->numShadowIndexesNoCaps;
		}
		external = true;
	} else {
		// must draw everything
		numIndexes = tri->numIndexes;
	}

	// set depth bounds
	if( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) {
		glDepthBoundsEXT( surf->scissorRect.zmin, surf->scissorRect.zmax );
	}

	// debug visualization
	if ( r_showShadows.GetInteger() ) {
		if ( r_showShadows.GetInteger() == 3 ) {
			if ( external ) {
				glColor3f( 0.1/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright );
			} else {
				// these are the surfaces that require the reverse
				glColor3f( 1/backEnd.overBright, 0.1/backEnd.overBright, 0.1/backEnd.overBright );
			}
		} else {
			// draw different color for turboshadows
			if ( surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE ) {
				if ( numIndexes == tri->numIndexes ) {
					glColor3f( 1/backEnd.overBright, 0.1/backEnd.overBright, 0.1/backEnd.overBright );
				} else {
					glColor3f( 1/backEnd.overBright, 0.4/backEnd.overBright, 0.1/backEnd.overBright );
				}
			} else {
				if ( numIndexes == tri->numIndexes ) {
					glColor3f( 0.1/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright );
				} else if ( numIndexes == tri->numShadowIndexesNoFrontCaps ) {
					glColor3f( 0.1/backEnd.overBright, 1/backEnd.overBright, 0.6/backEnd.overBright );
				} else {
					glColor3f( 0.6/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright );
				}
			}
		}

		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
		glDisable( GL_STENCIL_TEST );
		GL_Cull( CT_TWO_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
		GL_Cull( CT_FRONT_SIDED );
		glEnable( GL_STENCIL_TEST );

		return;
	}

	// patent-free work around
	if ( !external ) {
		// "preload" the stencil buffer with the number of volumes
		// that get clipped by the near or far clip plane
		glStencilOp( GL_KEEP, tr.stencilDecr, tr.stencilDecr );
		GL_Cull( CT_FRONT_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
		glStencilOp( GL_KEEP, tr.stencilIncr, tr.stencilIncr );
		GL_Cull( CT_BACK_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
	}

	// traditional depth-pass stencil shadows
	glStencilOp( GL_KEEP, GL_KEEP, tr.stencilIncr );
	GL_Cull( CT_FRONT_SIDED );
	RB_DrawShadowElementsWithCounters( tri, numIndexes );

	glStencilOp( GL_KEEP, GL_KEEP, tr.stencilDecr );
	GL_Cull( CT_BACK_SIDED );
	RB_DrawShadowElementsWithCounters( tri, numIndexes );
}

/*
=====================
RB_StencilShadowPass

Stencil test should already be enabled, and the stencil buffer should have
been set to 128 on any surfaces that might receive shadows
=====================
*/
void RB_StencilShadowPass( const drawSurf_t *drawSurfs ) {
	if ( !r_shadows.GetBool() ) {
		return;
	}

	if ( !drawSurfs ) {
		return;
	}

	RB_LogComment( "---------- RB_StencilShadowPass ----------\n" );

	globalImages->BindNull();
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// for visualizing the shadows
	if ( r_showShadows.GetInteger() ) {
		if ( r_showShadows.GetInteger() == 2 ) {
			// draw filled in
			GL_State( GLS_DEPTHMASK | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_LESS  );
		} else {
			// draw as lines, filling the depth buffer
			GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ZERO | GLS_POLYMODE_LINE | GLS_DEPTHFUNC_ALWAYS  );
		}
	} else {
		// don't write to the color buffer, just the stencil buffer
		GL_State( GLS_DEPTHMASK | GLS_COLORMASK | GLS_ALPHAMASK | GLS_DEPTHFUNC_LESS );
	}

	if ( r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat() ) {
		glPolygonOffset( r_shadowPolygonFactor.GetFloat(), -r_shadowPolygonOffset.GetFloat() );
		glEnable( GL_POLYGON_OFFSET_FILL );
	}

	glStencilFunc( GL_ALWAYS, 1, 255 );

	if ( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) {
		glEnable( GL_DEPTH_BOUNDS_TEST_EXT );
	}

	RB_RenderDrawSurfChainWithFunction( drawSurfs, RB_T_Shadow );

	GL_Cull( CT_FRONT_SIDED );

	if ( r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat() ) {
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	if ( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) {
		glDisable( GL_DEPTH_BOUNDS_TEST_EXT );
	}

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glStencilFunc( GL_GEQUAL, 128, 255 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
}



/*
=============================================================================================

BLEND LIGHT PROJECTION

=============================================================================================
*/

/*
=====================
RB_T_BlendLight

=====================
*/
static void RB_T_BlendLight( const drawSurf_t *surf ) {
	const srfTriangles_t *tri;

	tri = surf->geo;

	if ( backEnd.currentSpace != surf->space ) {
		idPlane	lightProject[4];
		int		i;

		for ( i = 0 ; i < 4 ; i++ ) {
			R_GlobalPlaneToLocal( surf->space->modelMatrix, backEnd.vLight->lightProject[i], lightProject[i] );
		}

		GL_SelectTexture( 0 );
		glTexGenfv( GL_S, GL_OBJECT_PLANE, lightProject[0].ToFloatPtr() );
		glTexGenfv( GL_T, GL_OBJECT_PLANE, lightProject[1].ToFloatPtr() );
		glTexGenfv( GL_Q, GL_OBJECT_PLANE, lightProject[2].ToFloatPtr() );

		GL_SelectTexture( 1 );
		glTexGenfv( GL_S, GL_OBJECT_PLANE, lightProject[3].ToFloatPtr() );
	}

	// this gets used for both blend lights and shadow draws
	if ( tri->ambientCache ) {
		idDrawVert	*ac = (idDrawVert *)vertexCache.Position( tri->ambientCache );
		glVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
	} else if ( tri->shadowCache ) {
		shadowCache_t	*sc = (shadowCache_t *)vertexCache.Position( tri->shadowCache );
		glVertexPointer( 3, GL_FLOAT, sizeof( shadowCache_t ), sc->xyz.ToFloatPtr() );
	}

	RB_DrawElementsWithCounters( tri );
}

//========================================================================

static idPlane	fogPlanes[4];

/*
=====================
RB_T_BasicFog

=====================
*/
static void RB_T_BasicFog( const drawSurf_t *surf ) {
	if ( backEnd.currentSpace != surf->space ) {
		idPlane	local;

		GL_SelectTexture( 0 );

		R_GlobalPlaneToLocal( surf->space->modelMatrix, fogPlanes[0], local );
		local[3] += 0.5;
		glTexGenfv( GL_S, GL_OBJECT_PLANE, local.ToFloatPtr() );

//		R_GlobalPlaneToLocal( surf->space->modelMatrix, fogPlanes[1], local );
//		local[3] += 0.5;
local[0] = local[1] = local[2] = 0; local[3] = 0.5;
		glTexGenfv( GL_T, GL_OBJECT_PLANE, local.ToFloatPtr() );

		GL_SelectTexture( 1 );

		// GL_S is constant per viewer
		R_GlobalPlaneToLocal( surf->space->modelMatrix, fogPlanes[2], local );
		local[3] += FOG_ENTER;
		glTexGenfv( GL_T, GL_OBJECT_PLANE, local.ToFloatPtr() );

		R_GlobalPlaneToLocal( surf->space->modelMatrix, fogPlanes[3], local );
		glTexGenfv( GL_S, GL_OBJECT_PLANE, local.ToFloatPtr() );
	}

	RB_T_RenderTriangleSurface( surf );
}



/*
==================
RB_FogPass
==================
*/
static void RB_FogPass( const drawSurf_t *drawSurfs,  const drawSurf_t *drawSurfs2 ) {
	
}


/*
==================
RB_STD_FogAllLights
==================
*/
void RB_STD_FogAllLights( void ) {
	viewLight_t	*vLight;

	if ( r_skipFogLights.GetBool() || r_showOverDraw.GetInteger() != 0 
		 || backEnd.viewDef->isXraySubview /* dont fog in xray mode*/
		 ) {
		return;
	}

	RB_LogComment( "---------- RB_STD_FogAllLights ----------\n" );

	glDisable( GL_STENCIL_TEST );

	for ( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next ) {
		backEnd.vLight = vLight;

#if 0 // _D3XP disabled that
		if ( r_ignore.GetInteger() ) {
			// we use the stencil buffer to guarantee that no pixels will be
			// double fogged, which happens in some areas that are thousands of
			// units from the origin
			backEnd.currentScissor = vLight->scissorRect;
			if ( r_useScissor.GetBool() ) {
				glScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
					backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
					backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
					backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}
			glClear( GL_STENCIL_BUFFER_BIT );

			glEnable( GL_STENCIL_TEST );

			// only pass on the cleared stencil values
			glStencilFunc( GL_EQUAL, 128, 255 );

			// when we pass the stencil test and depth test and are going to draw,
			// increment the stencil buffer so we don't ever draw on that pixel again
			glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
		}
#endif

		//if ( vLight->lightShader->IsFogLight() ) {
		//	RB_FogPass( vLight->globalInteractions, vLight->localInteractions );
		//} else if ( vLight->lightShader->IsBlendLight() ) {
		//	RB_BlendLight( vLight->globalInteractions, vLight->localInteractions );
		//}
		glDisable( GL_STENCIL_TEST );
	}

	glEnable( GL_STENCIL_TEST );
}

//=========================================================================================

/*
==================
RB_STD_LightScale

Perform extra blending passes to multiply the entire buffer by
a floating point value
==================
*/
void RB_STD_LightScale( void ) {
	float	v, f;

	if ( backEnd.overBright == 1.0f ) {
		return;
	}

	if ( r_skipLightScale.GetBool() ) {
		return;
	}

	RB_LogComment( "---------- RB_STD_LightScale ----------\n" );

	// the scissor may be smaller than the viewport for subviews
	if ( r_useScissor.GetBool() ) {
		glScissor( backEnd.viewDef->viewport.x1 + backEnd.viewDef->scissor.x1, 
			backEnd.viewDef->viewport.y1 + backEnd.viewDef->scissor.y1, 
			backEnd.viewDef->scissor.x2 - backEnd.viewDef->scissor.x1 + 1,
			backEnd.viewDef->scissor.y2 - backEnd.viewDef->scissor.y1 + 1 );
		backEnd.currentScissor = backEnd.viewDef->scissor;
	}

	// full screen blends
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity(); 
    glOrtho( 0, 1, 0, 1, -1, 1 );

	GL_State( GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_SRC_COLOR );
	GL_Cull( CT_TWO_SIDED );	// so mirror views also get it
	globalImages->BindNull();
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_STENCIL_TEST );

	v = 1;
	while ( idMath::Fabs( v - backEnd.overBright ) > 0.01 ) {	// a little extra slop
		f = backEnd.overBright / v;
		f /= 2;
		if ( f > 1 ) {
			f = 1;
		}
		glColor3f( f, f, f );
		v = v * f * 2;

		glBegin( GL_QUADS );
		glVertex2f( 0,0 );	
		glVertex2f( 0,1 );
		glVertex2f( 1,1 );	
		glVertex2f( 1,0 );	
		glEnd();
	}


	glPopMatrix();
	glEnable( GL_DEPTH_TEST );
	glMatrixMode( GL_MODELVIEW );
	GL_Cull( CT_FRONT_SIDED );
}


/*
================
RB_SetVertexParm
================
*/
void RB_SetVertexParm(renderParm_t rp, const float * value) {
	renderProgManager.SetUniformValue(rp, value);
}

/*
================
RB_SetVertexParms
================
*/
void RB_SetVertexParms(renderParm_t rp, const float * value, int num) {
	for (int i = 0; i < num; i++) {
		renderProgManager.SetUniformValue((renderParm_t)(rp + i), value + (i * 4));
	}
}

/*
================
RB_SetFragmentParm
================
*/
void RB_SetFragmentParm(renderParm_t rp, const float * value) {
	renderProgManager.SetUniformValue(rp, value);
}

/*
================
RB_SetMVP
================
*/
void RB_SetMVP(const idRenderMatrix & mvp) {
	RB_SetVertexParms(RENDERPARM_MVPMATRIX_X, mvp[0], 4);
}

/*
================
RB_SetShadowMatrix
================
*/
void RB_SetShadowMatrix(const idRenderMatrix& mvp) {
	RB_SetVertexParms(RENDERPARM_SHADOWMATRIX_X, mvp[0], 4);
}

/*
================
RB_SetModelMatrix
================
*/
void RB_SetModelMatrix(const idRenderMatrix &modelMatrix) {
	RB_SetVertexParms(RENDERPARM_MODELMATRIX_X, modelMatrix[0], 4);
}

/*
================
RB_SetModelMatrix
================
*/
void RB_SetModelMatrix(const float *modelMatrix) {
	idRenderMatrix m;
	idRenderMatrix transposed;
	memcpy(m.GetFloatPtr(), modelMatrix, sizeof(float) * 16);
	
	idRenderMatrix::Transpose(m, transposed);
	RB_SetModelMatrix(transposed);
}

//=========================================================================================

/*
=============
RB_STD_DrawView

=============
*/
void	RB_STD_DrawView( void ) {
	drawSurf_t	 **drawSurfs;
	int			numDrawSurfs;

	RB_LogComment( "---------- RB_STD_DrawView ----------\n" );

	backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;

	drawSurfs = (drawSurf_t **)&backEnd.viewDef->drawSurfs[0];
	numDrawSurfs = backEnd.viewDef->numDrawSurfs;

	if(backEnd.feedbackRenderTexture)
	{
		float screenCorrectionParm[4];
		screenCorrectionParm[0] = 1.0f / (float)renderSystem->GetScreenWidth();
		screenCorrectionParm[1] = 1.0f / (float)renderSystem->GetScreenHeight();
		screenCorrectionParm[2] = (float)renderSystem->GetScreenWidth();
		screenCorrectionParm[3] = (float)renderSystem->GetScreenHeight();
		RB_SetFragmentParm(RENDERPARM_SCREENCORRECTIONFACTOR, screenCorrectionParm);

		RB_SetVertexParms(RENDERPARM_UNPROJMATRIX_X, backEnd.viewDef->unprojectionToCameraRenderMatrix[0], 4);
	}

	// Render the shadow maps. 
	RB_Draw_ShadowMaps();

	// Update the global material parms.
	{
		idVec4 globalMaterialParms;
		globalMaterialParms.x = backEnd.viewDef->floatTime;
		globalMaterialParms.y = 0;
		globalMaterialParms.z = 0;
		globalMaterialParms.w = 0;

		renderProgManager.SetUniformValue(RENDERPARM_MATERIALPARMS, globalMaterialParms.ToFloatPtr());
	}

	// If we have a feedback render pass, lets do that now.
	if (backEnd.feedbackRenderTexture)
	{
		backEnd.feedbackRenderTexture->MakeCurrent();

		idScreenRect currentScreenRect = backEnd.viewDef->viewport;
		((viewDef_t *)backEnd.viewDef)->viewport.x1 = 0;
		((viewDef_t *)backEnd.viewDef)->viewport.x2 = backEnd.feedbackRenderTexture->GetWidth();
		((viewDef_t *)backEnd.viewDef)->viewport.y1 = 0;
		((viewDef_t *)backEnd.viewDef)->viewport.y2 = backEnd.feedbackRenderTexture->GetHeight();

		RB_BeginDrawingView();

		// draw the feedback pass. 
		RB_STD_DrawFeedbackPass(drawSurfs, numDrawSurfs);

		((viewDef_t *)backEnd.viewDef)->viewport = currentScreenRect;

		idRenderTexture::BindNull();
	}

	// If we have a backend rendertexture, assign it here.
	if (backEnd.renderTexture)
	{
		backEnd.renderTexture->MakeCurrent();

		idVec4 value;
		value.x = backEnd.renderTexture->GetWidth();
		value.y = backEnd.renderTexture->GetHeight();
		renderProgManager.SetUniformValue(RENDERPARM_RENDERTARGETSIZE, value.ToFloatPtr());
	}

	// clear the z buffer, set the projection matrix, etc
	RB_BeginDrawingView();

	// decide how much overbrighting we are going to do
	RB_DetermineLightScale();

	// Draw the sky portal
	{
		backEnd.currentClassDrawType = RENDER_CLASS_SKYPORTAL;
		RB_STD_FillDepthBuffer(drawSurfs, numDrawSurfs);
		RB_Interaction_DrawInteractions();

		// disable stencil shadow test
		glStencilFunc(GL_ALWAYS, 128, 255);

		// uplight the entire screen to crutch up not having better blending range
		//RB_STD_LightScale();

		// now draw any non-light dependent shading passes
		int	processed = RB_STD_DrawShaderPasses(drawSurfs, numDrawSurfs);

		// Clear the depth buffer.
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	// Draw the world.
	{
		backEnd.currentClassDrawType = RENDER_CLASS_WORLD;
		// fill the depth buffer and clear color buffer to black except on
		// subviews
		RB_STD_FillDepthBuffer(drawSurfs, numDrawSurfs);

		// Run Occlusion Queries for the lights.
		RB_Draw_LightOcclusion();

		// main light renderer
		RB_Interaction_DrawInteractions();
	}

	// disable stencil shadow test
	glStencilFunc( GL_ALWAYS, 128, 255 );

	// uplight the entire screen to crutch up not having better blending range
	//RB_STD_LightScale();

	// now draw any non-light dependent shading passes
	int	processed = RB_STD_DrawShaderPasses( drawSurfs, numDrawSurfs );

	// draw the particles.
	RB_STD_DrawParticles(drawSurfs, numDrawSurfs);

	// fob and blend lights
	RB_STD_FogAllLights();

	// now draw any post-processing effects using _currentRender
	if ( processed < numDrawSurfs ) {
		RB_STD_DrawShaderPasses( drawSurfs+processed, numDrawSurfs-processed );
	}

	RB_RenderDebugTools( drawSurfs, numDrawSurfs );
}
