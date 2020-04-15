// PortalSky.h
//

//
// rvmPortalSkyEntity_t
//
struct rvmPortalSkyEntity_t {
	rvmPortalSkyEntity_t();

	renderEntity_t renderEntity;
	qhandle_t renderEntityWorldId;

//	renderLight_t renderLight;
//	qhandle_t renderLightWorldId;
};

//
// rvmPortalSkyEntity_t::rvmPortalSkyEntity_t
//
ID_INLINE rvmPortalSkyEntity_t::rvmPortalSkyEntity_t() {
	renderEntityWorldId = -1;
	//renderLightWorldId = -1;
}

//
// rvmPortalSky
//
class rvmPortalSky {
public:
						rvmPortalSky();

	// Load a portal sky map.
	void				LoadPortalSkyMap(const char* name);

	// Frees the portal sky.
	void				FreePortalSky(void);
private:
	idMapFile*			mapFile;

	idList<rvmPortalSkyEntity_t> skyEntities;
};

extern rvmPortalSky		portalSky;