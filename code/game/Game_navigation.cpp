// Game_navigation.cpp
//

#include "Game_precompiled.h"
#include "Game_local.h"

#define DISTANCEFACTOR_CROUCH		1.3f		//crouch speed = 100
#define DISTANCEFACTOR_SWIM			1		//should be 0.66, swim speed = 150
#define DISTANCEFACTOR_WALK			0.33f	//walk speed = 300

/*
=================
idGameLocal::NavTravelTime
=================
*/
int idGameLocal::NavTravelTime(idVec3 start, idVec3 end)
{
	int intdist;
	float dist;
	idVec3 dir;

	dir = start - end;	
	dist = dir.Length();

	//if crouch only area
	dist *= DISTANCEFACTOR_WALK;

	intdist = (int)dist;
	//make sure the distance isn't zero
	if (intdist <= 0) 
		intdist = 1;
	return intdist;
}

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