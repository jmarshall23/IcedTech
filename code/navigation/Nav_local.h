// Nav_local.h
//

#include "Recast.h"
#include "../external/detour/Include/DetourNavMesh.h"
#include "../external/detour/Include/DetourNavMeshBuilder.h"
#include "../external/detour/Include/DetourNavMeshQuery.h"

struct rcPolyMesh;
struct dtNavMesh;
struct rcPolyMeshDetail;

#define NAV_FILE_VERSION				1
#define NAV_FILE_EXTENSION				".nav"

//
// navFileHeader_t
//
struct navFileHeader_t {
	int version;
	int mapCRC;
	int blobLength;
};

//
// rvmNavFileLocal
//
class rvmNavFileLocal : public rvmNavFile {
public:
	rvmNavFileLocal(const char *name);
	~rvmNavFileLocal();

	bool LoadFromFile(void);

	virtual const char *GetName() { return name.c_str(); }
	virtual void GetRandomPointNearPosition(idVec3 point, idVec3 &randomPoint);

	static void WriteNavFile(const char *name, rcPolyMesh *mesh, rcPolyMeshDetail *detailMesh, int mapCRC);
private:
	static void WriteNavToOBJ(const char *Name, rcPolyMeshDetail *mesh);

private:
	idStr name;
	dtNavMesh *m_navMesh;
	dtNavMeshQuery* m_navquery;
};

//
// rvmNavigationManager
//
class rvmNavigationManagerLocal : public rvmNavigationManager {
public:
	virtual rvmNavFile *LoadNavFile(const char *name);
	virtual void FreeNavFile(rvmNavFile *navFile);
private:
	idList<rvmNavFileLocal *> navMeshFiles;
};

extern rvmNavigationManagerLocal navigationManagerLocal;