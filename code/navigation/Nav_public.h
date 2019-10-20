// Nav_public.h
//

static const float m_cellSize = 4.0f;
static const float m_cellHeight = 0.2f;
static const float m_agentHeight = 2.0f;
static const float m_agentRadius = 0.6f;
static const float m_agentMaxClimb = 0.9f;
static const float m_agentMaxSlope = 45.0f;
static const float m_regionMinSize = 8;
static const float m_regionMergeSize = 20;
static const float m_edgeMaxLen = 12.0f;
static const float m_edgeMaxError = 1.3f;
static const float m_vertsPerPoly = 6.0f;
static const float m_detailSampleDist = 3.0f;
static const float m_detailSampleMaxError = 1.0f;

//
// rvmNavFile
//
class rvmNavFile {
public:
	// Returns the name of this navmesh.
	virtual const char *GetName() = 0;

	// Finds a random walkable point near point.
	virtual void GetRandomPointNearPosition(idVec3 point, idVec3 &randomPoint) = 0;
};

//
// rvmNavigationManager
//
class rvmNavigationManager {
public:
	// Loads in a navigation file.
	virtual rvmNavFile *LoadNavFile(const char *name) = 0;

	// Frees the navigation file.
	virtual void FreeNavFile(rvmNavFile *navFile) = 0;
private:

};

extern rvmNavigationManager *navigationManager;