// NavFile.cpp
//

#include "Engine_precompiled.h"
#include "Nav_local.h"

idCVar nav_dumpobj("nav_dumpobj", "0", CVAR_BOOL, "writes a obj represenation of the navmesh");

/*
===================
rvmNavFileLocal::rvmNavFileLocal
===================
*/
rvmNavFileLocal::rvmNavFileLocal(const char *name) {
	this->name = name;
	this->name.SetFileExtension(NAV_FILE_EXTENSION);
	m_navMesh = nullptr;
}

/*
===================
rvmNavFileLocal::~rvmNavFileLocal
===================
*/
rvmNavFileLocal::~rvmNavFileLocal() {
	if(m_navMesh != nullptr)
	{
		dtFree(m_navMesh);
		m_navMesh = nullptr;
	}
}

/*
===================
rvmNavFileLocal::GetRandomPointNearPosition
===================
*/
void rvmNavFileLocal::GetRandomPointNearPosition(idVec3 point, idVec3 &randomPoint) {
	//m_navquery->queryPolygons()
	//const dtMeshTile *meshTile = m_navMesh-
	//bool posOverPoly;
	//
	//dtPolyRef polyRef = m_navMesh->getPolyRefBase(meshTile);
	//
	//m_navquery->closestPointOnPoly(polyRef, point.ToFloatPtr(), randomPoint.ToFloatPtr(), &posOverPoly);
}

/*
===================
rvmNavFileLocal::LoadFromFile
===================
*/
bool rvmNavFileLocal::LoadFromFile(void) {
	idFileScoped file(fileSystem->OpenFileRead(name));
	navFileHeader_t header;
	
	if(file == nullptr)
	{
		common->Warning("Failed to open %s\n", name.c_str());
		return false;
	}
	
	file->Read(&header, sizeof(navFileHeader_t));
	if(header.version != NAV_FILE_VERSION) {
		common->Warning("Invalid header version\n");
		return false;
	}
	
	idTempArray<byte> navData(header.blobLength);
	file->Read(navData.Ptr(), header.blobLength);
	
	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		common->FatalError("Could not create Detour navmesh");
		return false;
	}
	
	dtStatus status;
	
	status = m_navMesh->init(navData.Ptr(), header.blobLength, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		common->FatalError("Could not init Detour navmesh");
		return false;
	}
	
	m_navquery = dtAllocNavMeshQuery();
	status = m_navquery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		common->FatalError("Could not init Detour navmesh query");
		return false;
	}

	return true;
}

/*
===================
rvmNavFileLocal::WriteNavToOBJ
===================
*/
void rvmNavFileLocal::WriteNavToOBJ(const char *name, rcPolyMeshDetail *mesh) {
	idStr navFileName;
	navFileName = "maps/";
	navFileName += name;
	navFileHeader_t header;

	navFileName.SetFileExtension("_navmesh.obj");

	// Open the nav file for writing.
	idFileScoped file(fileSystem->OpenFileWrite(navFileName));

	idList<idVec3> vertexes;
	for(int i = 0; i < mesh->nverts; i++)
	{
		idVec3 vertex;
		vertex.x = mesh->verts[(i * 3) + 0];
		vertex.y = mesh->verts[(i * 3) + 1];
		vertex.z = mesh->verts[(i * 3) + 2];
		vertexes.Append(vertex);
	}

	for (int i = 0; i < vertexes.Num(); i++)
	{
		file->Printf("v %f %f %f\n", vertexes[i].x, vertexes[i].y, vertexes[i].z);
	}

	file->Printf("\n");

	for (int i = 0; i < vertexes.Num(); i+=3)
		file->Printf("f %d %d %d\n", i + 1, i + 2, i + 3);
}

/*
===================
rvmNavFileLocal::WriteNavFile
===================
*/
void rvmNavFileLocal::WriteNavFile(const char *name, rcPolyMesh *mesh, rcPolyMeshDetail *detailMesh, int mapCRC) {
	idStr navFileName;
	navFileName = "maps/";
	navFileName += name;
	navFileHeader_t header;

	navFileName.SetFileExtension(NAV_FILE_EXTENSION);

	if(nav_dumpobj.GetBool())
	{
		WriteNavToOBJ(name, detailMesh);
	}

	// Open the nav file for writing.
	idFileScoped file(fileSystem->OpenFileWrite(navFileName));

	// Fill in the header.
	header.version = NAV_FILE_VERSION;
	header.mapCRC = mapCRC;

	// Write out the navmesh data.
	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));
	params.verts = mesh->verts;
	params.vertCount = mesh->nverts;
	params.polys = mesh->polys;
	params.polyAreas = mesh->areas;
	params.polyFlags = mesh->flags;
	params.polyCount = mesh->npolys;
	params.nvp = mesh->nvp;
	params.detailMeshes = detailMesh->meshes;
	params.detailVerts = detailMesh->verts;
	params.detailVertsCount = detailMesh->nverts;
	params.detailTris = detailMesh->tris;
	params.detailTriCount = detailMesh->ntris;
	//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
	//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
	//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
	//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
	//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
	//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
	//params.offMeshConCount = m_geom->getOffMeshConnectionCount();
	params.walkableHeight = m_agentHeight;
	params.walkableRadius = m_agentRadius;
	params.walkableClimb = m_agentMaxClimb;
	rcVcopy(params.bmin, mesh->bmin);
	rcVcopy(params.bmax, mesh->bmax);
	params.cs = m_cellSize;
	params.ch = m_cellHeight;
	params.buildBvTree = true;

	unsigned char* navData = 0;
	int navDataSize = 0;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
	{
		common->FatalError("Could not build Detour navmesh.");
		return;
	}

	header.blobLength = navDataSize;

	// Write out the header.
	file->Write(&header, sizeof(navFileHeader_t));

	// Write out the nav data.
	file->Write(navData, navDataSize);
}
