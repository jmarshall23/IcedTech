// PortalSky.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

/*
===========================================

Portal Sky System

The portal skies in Darklight Arena are created from simple maps. Only func_static and lights are supported
classes. Todo we should hook this up to the client entity system.

===========================================
*/

rvmPortalSky		portalSky;

/*
=====================
rvmPortalSky::rvmPortalSky
=====================
*/
rvmPortalSky::rvmPortalSky() {
	mapFile = NULL;
	skyEntities.Clear();
}

/*
=====================
rvmPortalSky::LoadPortalSkyMap
=====================
*/
void rvmPortalSky::LoadPortalSkyMap(const char* name) {
	if(mapFile != NULL) {
		common->FatalError("Portal sky already loaded and wasn't freed!\n");
		return;
	}

	// Load the map file.
	mapFile = new idMapFile();
	if(!mapFile->Parse(va("maps/%s", name))) {
		delete mapFile;
		gameLocal.Error("Failed to load portal sky %s\n", name);
		return;
	}

	// Spawn map entities and lights
	for(int i = 0; i < mapFile->GetNumEntities(); i++) {
		idMapEntity* entity = mapFile->GetEntity(i);
		idStr classname = entity->epairs.GetString("classname");
		rvmPortalSkyEntity_t  portalSkyEntity;

		if(classname == "func_static") {
			gameEdit->ParseSpawnArgsToRenderEntity(&entity->epairs, &portalSkyEntity.renderEntity);
			portalSkyEntity.renderEntity.classType = RENDER_CLASS_SKYPORTAL;
			portalSkyEntity.renderEntityWorldId = gameRenderWorld->AddEntityDef(&portalSkyEntity.renderEntity);
		}
		else if(classname == "light") {
			//gameEdit->ParseSpawnArgsToRenderLight(&entity->epairs, &portalSkyEntity.renderLight);
			//portalSkyEntity.renderLight.classType = RENDER_CLASS_SKYPORTAL;
			//portalSkyEntity.renderLightWorldId = gameRenderWorld->AddLightDef(&portalSkyEntity.renderLight);
		}
		else {
			common->Warning("Unknown portal sky entity class %s\n", classname.c_str());
			continue;
		}

		skyEntities.Append(portalSkyEntity);
	}
}

/*
=====================
rvmPortalSky::FreePortalSky
=====================
*/
void rvmPortalSky::FreePortalSky(void) {
	if (mapFile == NULL) {
		return;
	}

	delete mapFile;
	mapFile = NULL;
}