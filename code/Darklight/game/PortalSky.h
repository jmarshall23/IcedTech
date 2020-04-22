// PortalSky.h
//

//
// rvmPortalSkyEntity_t
//
struct rvmPortalSkyEntity_t {
	rvmPortalSkyEntity_t();

	idRenderEntity* renderEntity;
	idRenderLight* renderLight;
};

//
// rvmPortalSkyEntity_t::rvmPortalSkyEntity_t
//
ID_INLINE rvmPortalSkyEntity_t::rvmPortalSkyEntity_t() {
	renderEntity = NULL;
	renderLight = NULL;
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