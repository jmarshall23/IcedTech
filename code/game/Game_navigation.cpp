// Game_navigation.cpp
//

#include "game_precompiled.h"
#include "Game_local.h"

/*
=========================
idGameLocal::NavGetPathBetweenPoints
=========================
*/
bool idGameLocal::NavGetPathBetweenPoints(const idVec3 p1, const idVec3 p2, idList<idVec3>& points) {
	if (!navMeshFile) {
		return false;
	}

	if (!navMeshFile->GetPathBetweenPoints(p1, p2, points)) {
		return false;
	}

	if (points.Num() < 2)
		return false;


	// Let's do one more check, to ensure we can actually reach the goal.
	trace_t trace;
	idMat3 axis;
	axis.Identity();
	clip.Translation(trace, points[points.Num() - 2], p2, NULL, axis, CONTENTS_SOLID, NULL);

	if (trace.fraction != 1.0f)
		return false;

	return true;
}

/*
=========================
idGameLocal::NavGetPathBetweenPoints
=========================
*/
void idGameLocal::GetRandomPointNearPosition(idVec3 point, idVec3& randomPoint, float radius) {
	navMeshFile->GetRandomPointNearPosition(point, randomPoint, radius);
}

/*
=========================
idGameLocal::LoadMapNav
=========================
*/
bool idGameLocal::LoadMapNav(const char* mapName) {
	idStr navFileName = mapName;
	navFileName.SetFileExtension(NAV_FILE_EXTENSION);

	navMeshFile = navigationManager->LoadNavFile(navFileName);

	if (!navMeshFile) {
		common->Warning("Failed to load nav file!");
		return false;
	}

	return true;
}