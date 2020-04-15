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

#include "tr_local.h"

idRenderEntityLocal::idRenderEntityLocal() {
	memset( &parms, 0, sizeof( parms ) );
	memset( modelMatrix, 0, sizeof( modelMatrix ) );

	world					= NULL;
	index					= 0;
	lastModifiedFrameNum	= 0;
	archived				= false;
	dynamicModel			= NULL;
	dynamicModelFrameCount	= 0;
	cachedDynamicModel		= NULL;
	referenceBounds			= bounds_zero;
	viewCount				= 0;
	viewEntity				= NULL;
	visibleCount			= 0;
	decals					= NULL;
	overlay					= NULL;
	firstInteraction		= NULL;
	lastInteraction			= NULL;
	needsPortalSky			= false;
}

void idRenderEntityLocal::FreeRenderEntity() {
}

void idRenderEntityLocal::UpdateRenderEntity( const renderEntity_t *re, bool forceUpdate ) {
}

void idRenderEntityLocal::GetRenderEntity( renderEntity_t *re ) {
}

void idRenderEntityLocal::ForceUpdate() {
}

int idRenderEntityLocal::GetIndex() {
	return index;
}

void idRenderEntityLocal::ProjectOverlay( const idPlane localTextureAxis[2], const idMaterial *material ) {
}
void idRenderEntityLocal::RemoveDecals() {
}

//======================================================================

/*
==========================
idRenderLightParms::idRenderLightParms
==========================
*/
idRenderLightParms::idRenderLightParms()
{
	lightChannel = 0;
	axis.Zero();
	origin.Zero();
	uniqueLightId = -1;
	suppressLightInViewID = -1;
	allowLightInViewID = -1;
	noShadows = false;
	noSpecular = false;
	pointLight = false;
	parallel = false;
	lightRadius.Zero();
	lightCenter.Zero();
	ambientLight = false;
	target.Zero();
	right.Zero();
	up.Zero();
	start.Zero();
	end.Zero();
	prelightModel = NULL;
	lightId = 0;
	shader = NULL;
	referenceSound = NULL;
	dynamicShadows = false;
	name = "";
	classType = RENDER_CLASS_WORLD;

	for (int i = 0; i < MAX_ENTITY_SHADER_PARMS; i++)
		shaderParms[i] = 0;
}

/*
==========================
idRenderLightLocal::idRenderLightLocal
==========================
*/
idRenderLightLocal::idRenderLightLocal() {
	memset(&parms, 0, sizeof(parms));
	memset( modelMatrix, 0, sizeof( modelMatrix ) );
	memset( shadowFrustums, 0, sizeof( shadowFrustums ) );
	memset( lightProject, 0, sizeof( lightProject ) );
	memset( frustum, 0, sizeof( frustum ) );
	memset( frustumWindings, 0, sizeof( frustumWindings ) );

	SetLightChannel(0, true);

	parmsDirty = true;

	lightHasMoved			= false;
	world					= NULL;
	index					= 0;
	visibleFrame			= 0;
	areaNum					= 0;
	lastModifiedFrameNum	= 0;
	currentOcclusionQuery   = NULL;
	archived				= false;
	lightShader				= NULL;
	falloffImage			= NULL;
	globalLightOrigin		= vec3_zero;
	frustumTris				= NULL;
	numShadowFrustums		= 0;
	viewCount				= 0;
	viewLight				= NULL;
	foggedPortals			= NULL;
	firstInteraction		= NULL;
	lastInteraction			= NULL;
}

/*
==========================
idRenderLightLocal::~idRenderLightLocal
==========================
*/
idRenderLightLocal::~idRenderLightLocal() {
	if (currentOcclusionQuery != NULL) {
		delete currentOcclusionQuery;
		currentOcclusionQuery = NULL;
	}

	R_FreeLightDefFrustum(this);
}

/*
==========================
idRenderLightLocal::SetLightChannel
==========================
*/
void idRenderLightLocal::SetLightChannel(int lightChannel, bool enabled) {
	if (enabled) {
		this->parms.lightChannel |= 1 << lightChannel;
	}
	else {
		this->parms.lightChannel &= ~(1UL << lightChannel);
	}
}

/*
==========================
idRenderLightLocal::HasLightChannel
==========================
*/
bool idRenderLightLocal::HasLightChannel(int lightChannel) {
	return (!!((this->parms.lightChannel) & (1ULL << (lightChannel))));
}

/*
==========================
idRenderLightLocal::ClearLightChannel
==========================
*/
void idRenderLightLocal::ClearLightChannel(void) {
	parms.lightChannel = 0;
}

/*
==========================
idRenderLightLocal::FreeRenderLight
==========================
*/
void idRenderLightLocal::FreeRenderLight() {
}

/*
==========================
idRenderLightLocal::UpdateRenderLight
==========================
*/
void idRenderLightLocal::UpdateRenderLight( bool forceUpdate ) {
	if (r_skipUpdates.GetBool()) {
		return;
	}

	tr.pc.c_lightUpdates++;
	
	lastModifiedFrameNum = tr.frameCount;
	if (session->writeDemo && archived) {
		world->WriteFreeLight(index);
		archived = false;
	}

	if (lightHasMoved) {
		parms.prelightModel = NULL;
	}

	if (parmsDirty)
	{
		R_FreeLightDefDerivedData(this);
		R_DeriveLightData(this);
		R_CreateLightRefs(this);
		R_CreateLightDefFogPortals(this);
	}
	parmsDirty = false;
}

/*
==========================
idRenderLightLocal::ForceUpdate
==========================
*/
void idRenderLightLocal::ForceUpdate() {
}

/*
==========================
idRenderLightLocal::GetIndex
==========================
*/
int idRenderLightLocal::GetIndex() {
	return index;
}
