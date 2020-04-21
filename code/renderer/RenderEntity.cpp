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

/*
=======================
idRenderEntityLocal::idRenderEntityLocal
=======================
*/
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
	parmsDirty = true;
	modelChanged = true;

	parms.lightChannel = 0;
	SetLightChannel(0, true);
	parms.hModel = nullptr;
	parms.joints = nullptr;
	parms.frameNum = 0;
	parms.hasDynamicShadows = false;
	parms.skipEntityViewCulling = false;
	parms.forceVirtualTextureHighQuality = false;
	parms.classType = RENDER_CLASS_WORLD;
}


/*
=========================
idRenderEntityLocal::SetLightChannel
=========================
*/
void idRenderEntityLocal::SetLightChannel(int lightChannel, bool enabled) {
	if (enabled) {
		parms.lightChannel |= 1 << lightChannel;
	}
	else {
		parms.lightChannel &= ~(1UL << lightChannel);
	}
}

/*
=========================
idRenderEntityLocal::HasLightChannel
=========================
*/
bool idRenderEntityLocal::HasLightChannel(int lightChannel) {
	return (!!((parms.lightChannel) & (1ULL << (lightChannel))));
}

/*
=========================
idRenderEntityLocal::HasValidJoints
=========================
*/
bool idRenderEntityLocal::HasValidJoints() const {
	return parms.joints != nullptr;
}

/*
=========================
idRenderEntityLocal::FreeRenderEntity
=========================
*/
void idRenderEntityLocal::FreeRenderEntity() {

}

/*
=========================
idRenderEntityLocal::UpdateRenderEntity
=========================
*/
void idRenderEntityLocal::UpdateRenderEntity( bool forceUpdate ) {
	if (r_skipUpdates.GetBool()) {
		return;
	}

	tr.pc.c_entityUpdates++;

	if (!parms.forceUpdate) {

		// check for exact match (OPTIMIZE: check through pointers more)
		if (!parms.joints && !parms.callbackData && !dynamicModel && !parmsDirty) {
			return;
		}

		// if the only thing that changed was shaderparms, we can just leave things as they are
		// after updating parms

		// if we have a callback function and the bounds, origin, axis and model match,
		// then we can leave the references as they are
		if (parms.callback) {			
			if (!parmsDirty) {
				// only clear the dynamic model and interaction surfaces if they exist
				R_ClearEntityDefDynamicModel(this);
				return;
			}
		}
	}

	// save any decals if the model is the same, allowing marks to move with entities
	if (!modelChanged) {
		R_FreeEntityDefDerivedData(this, true, true);
	}
	else {
		R_FreeEntityDefDerivedData(this, false, false);
	}

	R_AxisToModelMatrix(parms.axis, parms.origin, modelMatrix);

	lastModifiedFrameNum = tr.frameCount;
	if (session->writeDemo && archived) {
		world->WriteFreeEntity(index);
		archived = false;
	}

	// optionally immediately issue any callbacks
	if (!r_useEntityCallbacks.GetBool() && parms.callback) {
		R_IssueEntityDefCallback(this);
	}

	// based on the model bounds, add references in each area
	// that may contain the updated surface
	R_CreateEntityRefs(this);

	parmsDirty = false;
	modelChanged = false;
}

/*
=========================
idRenderEntityLocal::ForceUpdate
=========================
*/
void idRenderEntityLocal::ForceUpdate() {
}

/*
=========================
idRenderEntityLocal::GetIndex
=========================
*/
int idRenderEntityLocal::GetIndex() {
	return index;
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
	isEnabled = true;
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
	referenceSound = NULL;
	lightRenderType = LIGHT_RENDER_STATIC;
	name = "";
	classType = RENDER_CLASS_WORLD;

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
	parms.isEnabled = true;

	lightHasMoved			= false;
	world					= NULL;
	index					= 0;
	visibleFrame			= 0;
	areaNum					= 0;
	lastModifiedFrameNum	= 0;
	currentOcclusionQuery   = NULL;
	archived				= false;
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
