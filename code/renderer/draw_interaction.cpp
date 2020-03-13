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

#include "Engine_precompiled.h"
#pragma hdrstop

#include "tr_local.h"

/*
=========================================================================================

GENERAL INTERACTION RENDERING

=========================================================================================
*/

/*
==================
RB_Interaction_DrawInteraction
==================
*/
void	RB_Interaction_DrawInteraction( const drawInteraction_t *din ) {
	// load all the vertex program parameters
	Draw_SetVertexParm(RENDERPARM_LOCALLIGHTORIGIN, din->localLightOrigin.ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_LOCALVIEWORIGIN, din->localViewOrigin.ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_LIGHTPROJECTION_S, din->lightProjection[0].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_LIGHTPROJECTION_T, din->lightProjection[1].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_LIGHTPROJECTION_Q, din->lightProjection[2].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_LIGHTFALLOFF_S, din->lightProjection[3].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_BUMPMATRIX_S, din->bumpMatrix[0].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_BUMPMATRIX_T, din->bumpMatrix[1].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_DIFFUSEMATRIX_S, din->diffuseMatrix[0].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_DIFFUSEMATRIX_T, din->diffuseMatrix[1].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_SPECULARMATRIX_S, din->specularMatrix[0].ToFloatPtr() );
	Draw_SetVertexParm(RENDERPARM_SPECULARMATRIX_T, din->specularMatrix[1].ToFloatPtr() );

	// Set up the virtual texturing parametors.
	idVec4 virtualMapping;
	idVec4 virtualPageInfo;

	if (din->surf->material->IsVirtualTextured())
	{
		virtualMapping.x = din->surf->material->GetVirtualTextureHeightInPages();
		virtualMapping.y = VIRTUALTEXTURE_TILESIZE;
		virtualMapping.z = vt_ImageSize.GetInteger();
		virtualMapping.w = din->surf->material->GetVirtualTextureWidthInPages();

		virtualPageInfo.x = din->surf->material->GetVirtualAlbedoImage()->GetWidth(0);
		virtualPageInfo.y = din->surf->material->GetVirtualAlbedoImage()->GetHeight(0);
		virtualPageInfo.z = 0;
		virtualPageInfo.w = 0;
	}
	else
	{
		virtualMapping.Zero();
		virtualPageInfo.Zero();
	}
	Draw_SetVertexParm(RENDERPARM_VIRTUALMAPPING, virtualMapping.ToFloatPtr());
	Draw_SetVertexParm(RENDERPARM_VIRTUALPAGEINFO, virtualPageInfo.ToFloatPtr());

	static const float zero[4] = { 0, 0, 0, 0 };
	static const float one[4] = { 1, 1, 1, 1 };
	static const float negOne[4] = { -1, -1, -1, -1 };

	switch ( din->vertexColor ) {
	case SVC_IGNORE:
		Draw_SetVertexParm(RENDERPARM_VERTEXCOLOR_MODULATE, zero );
		Draw_SetVertexParm(RENDERPARM_VERTEXCOLOR_ADD, one );
		break;
	case SVC_MODULATE:
		Draw_SetVertexParm(RENDERPARM_VERTEXCOLOR_MODULATE, one);
		Draw_SetVertexParm(RENDERPARM_VERTEXCOLOR_ADD, zero);
		break;
	case SVC_INVERSE_MODULATE:
		Draw_SetVertexParm(RENDERPARM_VERTEXCOLOR_MODULATE, negOne);
		Draw_SetVertexParm(RENDERPARM_VERTEXCOLOR_ADD, one);
		break;
	}

	// set the constant colors
	RB_SetFragmentParm(RENDERPARM_DIFFUSEMODIFIER, din->diffuseColor.ToFloatPtr() );
	RB_SetFragmentParm(RENDERPARM_SPECULARMODIFIER, din->specularColor.ToFloatPtr() );

	// set the textures

	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( 0 );
	din->bumpImage->Bind();

	// texture 2 will be the light falloff texture
	GL_SelectTextureNoClient( 1 );
	din->lightFalloffImage->Bind();

	// texture 3 will be the light projection texture
	GL_SelectTextureNoClient( 2 );
	din->lightImage->Bind();

	// texture 4 is the per-surface diffuse map
	GL_SelectTextureNoClient( 3 );
	din->diffuseImage->Bind();

	// texture 5 is the per-surface specular map
	GL_SelectTextureNoClient( 4 );
	din->specularImage->Bind();

	if (din->surf->material->IsVirtualTextured())
	{
		GL_SelectTextureNoClient(5);
		din->surf->material->GetVirtualPageOffsetsImage()->Bind();
	}

	// texture 7 is the reflection cube map.
	GL_SelectTextureNoClient(6);
	if(din->surf->material->HasReflections() && din->surf->geo->reflectionCaptureImage) {
		din->surf->geo->reflectionCaptureImage->Bind();
	}
	else {
		globalImages->blackCubeMapImage->Bind();
	}

	// draw it
	RB_DrawElementsWithCounters( din->surf->geo );
}


/*
=============
RB_Interaction_CreateDrawInteractions

=============
*/
void RB_Interaction_CreateDrawInteractions( const drawSurf_t *surf ) {
	if ( !surf ) {
		return;
	}

	// perform setup here that will be constant for all interactions
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc );

	// bind the vertex program
	//if ( r_testARBProgram.GetBool() ) {
	//	glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_TEST );
	//	glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_TEST );
	//} else {
	//	glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_INTERACTION );
	//	glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_INTERACTION );
	//}

	//glEnable(GL_VERTEX_PROGRAM_ARB);
	//glEnable(GL_FRAGMENT_PROGRAM_ARB);

	renderProgManager.BindShader_Interaction();

	// enable the vertex arrays
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
	glEnableClientState( GL_COLOR_ARRAY );

	// texture 0 is the normalization cube map for the vector towards the light
	GL_SelectTextureNoClient( 0 );
	if ( backEnd.vLight->lightShader->IsAmbientLight() ) {
		globalImages->ambientNormalMap->Bind();
	} else {
		globalImages->normalCubeMapImage->Bind();
	}

	// texture 6 is the specular lookup table
	GL_SelectTextureNoClient( 6 );
	if ( r_testARBProgram.GetBool() ) {
		globalImages->specular2DTableImage->Bind();	// variable specularity in alpha channel
	} else {
		globalImages->specularTableImage->Bind();
	}


	for ( ; surf ; surf=surf->nextOnLight ) {
		// perform setup here that will not change over multiple interaction passes
		
		// set the vertex pointers
		idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_NORMAL, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
		//glVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_TANGENT, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_ST, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
		//glVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_VERTEX, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->xyz.ToFloatPtr());
		glVertexAttribPointerARB(PC_ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(idDrawVert), (void *)&ac->color);

		// this may cause RB_Interaction_DrawInteraction to be exacuted multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_CreateSingleDrawInteractions( surf, RB_Interaction_DrawInteraction );
	}

	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_ST);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_TANGENT);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_VERTEX);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_NORMAL);
	glDisableClientState( GL_COLOR_ARRAY );

	// disable features
	GL_SelectTextureNoClient( 6 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 5 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 4 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 3 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 2 );
	globalImages->BindNull();

	GL_SelectTextureNoClient( 1 );
	globalImages->BindNull();

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture( 0 );

	renderProgManager.Unbind();
}


/*
==================
RB_Interaction_DrawInteractions
==================
*/
void RB_Interaction_DrawInteractions( void ) {
	viewLight_t		*vLight;
	const idMaterial	*lightShader;

	GL_SelectTexture( 0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);

	//
	// for each light, perform adding and shadowing
	//
	for ( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next ) {
		backEnd.vLight = vLight;

		// do fogging later
		if ( vLight->lightShader->IsFogLight() ) {
			continue;
		}
		if ( vLight->lightShader->IsBlendLight() ) {
			continue;
		}

		if ( !vLight->localInteractions && !vLight->globalInteractions
			&& !vLight->translucentInteractions ) {
			continue;
		}

		lightShader = vLight->lightShader;

		// clear the stencil buffer if needed
		if ( vLight->globalShadows || vLight->localShadows ) {
			backEnd.currentScissor = vLight->scissorRect;
			if ( r_useScissor.GetBool() ) {
				glScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
					backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
					backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
					backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}
			glClear( GL_STENCIL_BUFFER_BIT );
		} else {
			// no shadows, so no need to read or write the stencil buffer
			// we might in theory want to use GL_ALWAYS instead of disabling
			// completely, to satisfy the invarience rules
			glStencilFunc( GL_ALWAYS, 128, 255 );
		}

		RB_StencilShadowPass(vLight->globalShadows);
		RB_Interaction_CreateDrawInteractions(vLight->localInteractions);
		RB_StencilShadowPass(vLight->localShadows);
		RB_Interaction_CreateDrawInteractions(vLight->globalInteractions);

		renderProgManager.Unbind();

		// translucent surfaces never get stencil shadowed
		if ( r_skipTranslucent.GetBool() ) {
			continue;
		}

		glStencilFunc( GL_ALWAYS, 128, 255 );

		backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
		RB_Interaction_CreateDrawInteractions( vLight->translucentInteractions );

		backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
	}

	// disable stencil shadow test
	glStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

//===================================================================================


typedef struct {
	GLenum			target;
	GLuint			ident;
	char			name[64];
} progDef_t;

static	const int	MAX_GLPROGS = 200;

// a single file can have both a vertex program and a fragment program
static progDef_t	progs[MAX_GLPROGS] = {
	{ GL_VERTEX_PROGRAM_ARB, VPROG_BUMPY_ENVIRONMENT, "bumpyEnvironment.vfp" },
	{ GL_FRAGMENT_PROGRAM_ARB, FPROG_BUMPY_ENVIRONMENT, "bumpyEnvironment.vfp" },
	{ GL_VERTEX_PROGRAM_ARB, VPROG_ENVIRONMENT, "environment.vfp" },
	{ GL_FRAGMENT_PROGRAM_ARB, FPROG_ENVIRONMENT, "environment.vfp" },
	{ GL_VERTEX_PROGRAM_ARB, VPROG_GLASSWARP, "arbVP_glasswarp.txt" },
	{ GL_FRAGMENT_PROGRAM_ARB, FPROG_GLASSWARP, "arbFP_glasswarp.txt" },

	// additional programs can be dynamically specified in materials
};

/*
=================
R_LoadARBProgram
=================
*/
void R_LoadARBProgram( int progIndex ) {
	int		ofs;
	int		err;
	idStr	fullPath = "glprogs/";
	fullPath += progs[progIndex].name;
	char	*fileBuffer;
	char	*buffer;
	char	*start, *end;

	common->Printf( "%s", fullPath.c_str() );

	// load the program even if we don't support it, so
	// fs_copyfiles can generate cross-platform data dumps
	fileSystem->ReadFile( fullPath.c_str(), (void **)&fileBuffer, NULL );
	if ( !fileBuffer ) {
		common->Printf( ": File not found\n" );
		return;
	}

	// copy to stack memory and free
	buffer = (char *)_alloca( strlen( fileBuffer ) + 1 );
	strcpy( buffer, fileBuffer );
	fileSystem->FreeFile( fileBuffer );

	if ( !glConfig.isInitialized ) {
		return;
	}

	//
	// submit the program string at start to GL
	//
	if ( progs[progIndex].ident == 0 ) {
		// allocate a new identifier for this program
		progs[progIndex].ident = PROG_USER + progIndex;
	}

	// vertex and fragment programs can both be present in a single file, so
	// scan for the proper header to be the start point, and stamp a 0 in after the end

	if ( progs[progIndex].target == GL_VERTEX_PROGRAM_ARB ) {
		if ( !glConfig.ARBVertexProgramAvailable ) {
			common->Printf( ": GL_VERTEX_PROGRAM_ARB not available\n" );
			return;
		}
		start = strstr( (char *)buffer, "!!ARBvp" );
	}
	if ( progs[progIndex].target == GL_FRAGMENT_PROGRAM_ARB ) {
		if ( !glConfig.ARBFragmentProgramAvailable ) {
			common->Printf( ": GL_FRAGMENT_PROGRAM_ARB not available\n" );
			return;
		}
		start = strstr( (char *)buffer, "!!ARBfp" );
	}
	if ( !start ) {
		common->Printf( ": !!ARB not found\n" );
		return;
	}
	end = strstr( start, "END" );

	if ( !end ) {
		common->Printf( ": END not found\n" );
		return;
	}
	end[3] = 0;

	glBindProgramARB( progs[progIndex].target, progs[progIndex].ident );
	glGetError();

	glProgramStringARB( progs[progIndex].target, GL_PROGRAM_FORMAT_ASCII_ARB,
		strlen( start ), (unsigned char *)start );

	err = glGetError();
	glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, (GLint *)&ofs );
	if ( err == GL_INVALID_OPERATION ) {
		const GLubyte *str = glGetString( GL_PROGRAM_ERROR_STRING_ARB );
		common->Printf( "\nGL_PROGRAM_ERROR_STRING_ARB: %s\n", str );
		if ( ofs < 0 ) {
			common->Printf( "GL_PROGRAM_ERROR_POSITION_ARB < 0 with error\n" );
		} else if ( ofs >= (int)strlen( (char *)start ) ) {
			common->Printf( "error at end of program\n" );
		} else {
			common->Printf( "error at %i:\n%s", ofs, start + ofs );
		}
		return;
	}
	if ( ofs != -1 ) {
		common->Printf( "\nGL_PROGRAM_ERROR_POSITION_ARB != -1 without error\n" );
		return;
	}

	common->Printf( "\n" );
}

/*
==================
R_FindARBProgram

Returns a GL identifier that can be bound to the given target, parsing
a text file if it hasn't already been loaded.
==================
*/
int R_FindARBProgram( GLenum target, const char *program ) {
	int		i;
	idStr	stripped = program;

	stripped.StripFileExtension();

	// see if it is already loaded
	for ( i = 0 ; progs[i].name[0] ; i++ ) {
		if ( progs[i].target != target ) {
			continue;
		}

		idStr	compare = progs[i].name;
		compare.StripFileExtension();

		if ( !idStr::Icmp( stripped.c_str(), compare.c_str() ) ) {
			return progs[i].ident;
		}
	}

	if ( i == MAX_GLPROGS ) {
		common->Error( "R_FindARBProgram: MAX_GLPROGS" );
	}

	// add it to the list and load it
	progs[i].ident = (program_t)0;	// will be gen'd by R_LoadARBProgram
	progs[i].target = target;
	strncpy( progs[i].name, program, sizeof( progs[i].name ) - 1 );

	R_LoadARBProgram( i );

	return progs[i].ident;
}

/*
==================
R_ReloadARBPrograms_f
==================
*/
void R_ReloadARBPrograms_f( const idCmdArgs &args ) {
	int		i;

	common->Printf( "----- R_ReloadARBPrograms -----\n" );
	for ( i = 0 ; progs[i].name[0] ; i++ ) {
		R_LoadARBProgram( i );
	}
	common->Printf( "-------------------------------\n" );
}

/*
==================
R_ARB2_Init

==================
*/
void R_ARB2_Init( void ) {
	glConfig.allowARB2Path = false;

	common->Printf( "---------- R_ARB2_Init ----------\n" );

	if ( !glConfig.ARBVertexProgramAvailable || !glConfig.ARBFragmentProgramAvailable ) {
		common->Printf( "Not available.\n" );
		return;
	}

	common->Printf( "Available.\n" );

	common->Printf( "---------------------------------\n" );

	glConfig.allowARB2Path = true;
}

