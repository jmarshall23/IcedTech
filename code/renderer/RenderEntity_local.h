// RenderEntity_local.h
//

//
// idRenderLightParms
//
struct idRenderLightParms {
	idRenderLightParms();

	idStr name;	

	renderClassWorldType_t	classType;

	idMat3					axis;				// rotation vectors, must be unit length
	idVec3					origin;

	int						lightChannel;

	int						uniqueLightId;		// light id that is unique to this light

	bool					dynamicShadows;		// if this light has dynamic shadows or not.

	// if non-zero, the light will not show up in the specific view,
	// which may be used if we want to have slightly different muzzle
	// flash lights for the player and other views
	int						suppressLightInViewID;

	// if non-zero, the light will only show up in the specific view
	// which can allow player gun gui lights and such to not effect everyone
	int						allowLightInViewID;

	// I am sticking the four bools together so there are no unused gaps in
	// the padded structure, which could confuse the memcmp that checks for redundant
	// updates
	bool					noShadows;			// (should we replace this with material parameters on the shader?)
	bool					noSpecular;			// (should we replace this with material parameters on the shader?)

	bool					pointLight;			// otherwise a projection light (should probably invert the sense of this, because points are way more common)
	bool					parallel;			// lightCenter gives the direction to the light at infinity
	idVec3					lightRadius;		// xyz radius for point lights
	idVec3					lightCenter;		// offset the lighting direction for shading and
												// shadows, relative to origin
	bool					ambientLight;		// light is ambient only

	// frustum definition for projected lights, all reletive to origin
	// FIXME: we should probably have real plane equations here, and offer
	// a helper function for conversion from this format
	idVec3					target;
	idVec3					right;
	idVec3					up;
	idVec3					start;
	idVec3					end;

	// Dmap will generate an optimized shadow volume named _prelight_<lightName>
	// for the light against all the _area* models in the map.  The renderer will
	// ignore this value if the light has been moved after initial creation
	idRenderModel* prelightModel;

	// muzzle flash lights will not cast shadows from player and weapon world models
	int						lightId;


	const idMaterial* shader;				// NULL = either lights/defaultPointLight or lights/defaultProjectedLight
	float					shaderParms[MAX_ENTITY_SHADER_PARMS];		// can be used in any way by shader
	idSoundEmitter* referenceSound;		// for shader sound tables, allowing effects to vary with sounds
};

class idRenderLightLocal : public idRenderLight {
public:
	idRenderLightLocal();
	~idRenderLightLocal();

	virtual void			FreeRenderLight();
	virtual void			UpdateRenderLight(bool forceUpdate = false);
	virtual void			ForceUpdate();
	virtual int				GetIndex();
	virtual void			SetLightChannel(int lightChannel, bool enabled);
	virtual bool			HasLightChannel(int lightChannel);
	virtual void			ClearLightChannel(void);
public:
	virtual void			SetName(const char* name) { parms.name = name; }
	virtual const char*		GetName(void) { return parms.name.c_str(); }

	virtual idMat3			GetAxis(void) { return parms.axis; }
	virtual void			SetAxis(idMat3 axis) {
		parms.axis = axis; 
		parmsDirty = true;
	}
	
	virtual idVec3			GetOrigin(void) { return parms.origin; }
	virtual void			SetOrigin(idVec3 origin) { 
		parms.origin = origin;
		parmsDirty = true;
	}

	virtual int				GetUniqueLightId(void) { return parms.uniqueLightId; }
	virtual void			SetUniqueLightId(int uniqueLightId) { 
		parms.uniqueLightId = uniqueLightId; 
		parmsDirty = true;
	}

	virtual bool			GetDynamicShadows(void) { return parms.dynamicShadows; }
	virtual void			SetDynamicShadows(bool dynamicShadows) {
		parms.dynamicShadows = dynamicShadows; 
		parmsDirty = true;
	}

	virtual int				GetSurpressLightInViewID(void) { return parms.suppressLightInViewID; }
	virtual void			SetSurpressLightInViewID(int suppressLightInViewID) { 
		parms.suppressLightInViewID = suppressLightInViewID; 
		parmsDirty = true;
	}

	virtual int				GetAllowLightInViewID(void) { return parms.allowLightInViewID; }
	virtual void			SetAllowLightInViewID(int allowLightInViewID) { 
		parms.allowLightInViewID = allowLightInViewID; 
		parmsDirty = true;
	}

	virtual bool			GetNoShadows(void) { return parms.noShadows; }
	virtual void			SetNoShadows(bool noShadows) { 
		parms.noShadows = noShadows;
		parmsDirty = true;
	}

	virtual bool			GetNoSpecular(void) { return parms.noSpecular; }
	virtual void			SetNoSpecular(bool noSpecular) { 
		parms.noSpecular = noSpecular; 
		parmsDirty = true;
	}

	virtual bool			GetPointLight(void) { return parms.pointLight; }
	virtual void			SetPointLight(bool pointLight) { 
		parms.pointLight = pointLight; 
		parmsDirty = true;
	}

	virtual bool			GetParallelLight(void) { return parms.parallel; }
	virtual void			SetParallelLight(bool parallel) { 
		parms.parallel = parallel; 
		parmsDirty = true;
	}

	virtual idVec3			GetLightRadius(void) { return parms.lightRadius; }
	virtual void			SetLightRadius(idVec3 lightRadius) { 
		parms.lightRadius = lightRadius; 
		parmsDirty = true;
	}

	virtual idVec3			GetLightCenter(void) { return parms.lightCenter; }
	virtual void			SetLightCenter(idVec3 lightCenter) { 
		parms.lightCenter = lightCenter; 
		parmsDirty = true;
	}

	virtual bool			GetAmbientLight(void) { return parms.ambientLight; }
	virtual void			SetAmbientLight(bool ambientLight) { 
		parms.ambientLight = ambientLight; 
		parmsDirty = true;
	}

	virtual idVec3			GetTarget(void) { return parms.target; }
	virtual void			SetTarget(idVec3 target) { 
		parms.target = target; 
		parmsDirty = true;
	}

	virtual idVec3			GetRight(void) { return parms.right; }
	virtual void			SetRight(idVec3 right) { 
		parms.right = right; 
		parmsDirty = true;
	}

	virtual idVec3			GetUp(void) { return parms.up; }
	virtual void			SetUp(idVec3 up) { 
		parms.up = up; 
		parmsDirty = true;
	}

	virtual idVec3			GetStart(void) { return parms.start; }
	virtual void			SetStart(idVec3 start) { 
		parms.start = start; 
		parmsDirty = true;
	}

	virtual idVec3			GetEnd(void) { return parms.end; }
	virtual void			SetEnd(idVec3 end) { 
		parms.end = end; 
		parmsDirty = true;
	}

	virtual int				GetLightId(void) { return parms.lightId; }
	virtual void			SetLightId(int lightId) { 
		parms.lightId = lightId; 
		parmsDirty = true;
	}

	virtual const idMaterial* GetShader(void) { return parms.shader; }
	virtual void			  SetShader(const idMaterial* material) { 
		parms.shader = material; 
		parmsDirty = true;
	}

	virtual float			GetShaderParam(int index) { return parms.shaderParms[index]; }
	virtual void			SetShaderParam(int index, float value) { 
		parms.shaderParms[index] = value; 
		parmsDirty = true;
	}

	virtual idSoundEmitter* GetReferenceSound(void) { return parms.referenceSound; }
	virtual void			SetReferenceSound(idSoundEmitter* referenceSound) { 
		parms.referenceSound = referenceSound; 
		parmsDirty = true;
	}
public:
	// Frame that the light was visible.
	int						visibleFrame;

	bool					parmsDirty;

	idRenderLightParms		parms;					// specification

	bool					lightHasMoved;			// the light has changed its position since it was
													// first added, so the prelight model is not valid

	float					modelMatrix[16];		// this is just a rearrangement of parms.axis and parms.origin

	idRenderWorldLocal* world;
	int						index;					// in world lightdefs

	int						areaNum;				// if not -1, we may be able to cull all the light's
													// interactions if !viewDef->connectedAreas[areaNum]

	int						lastModifiedFrameNum;	// to determine if it is constantly changing,
													// and should go in the dynamic frame memory, or kept
													// in the cached memory
	bool					archived;				// for demo writing

	class rvmOcclusionQuery* currentOcclusionQuery;

	// derived information
	idPlane					lightProject[4];
	idRenderMatrix			baseLightProject;		// global xyz1 to projected light strq
	idRenderMatrix			inverseBaseLightProject;// transforms the zero-to-one cube to exactly cover the light in world space


	const idMaterial* lightShader;			// guaranteed to be valid, even if parms.shader isn't
	idImage* falloffImage;

	idVec3					globalLightOrigin;		// accounting for lightCenter and parallel


	idPlane					frustum[6];				// in global space, positive side facing out, last two are front/back
	idPlane					previousFrustum[6];		// in global space, positive side facing out, last two are front/back
	idWinding* frustumWindings[6];		// used for culling
	srfTriangles_t* frustumTris;			// triangulated frustumWindings[]

	int						numShadowFrustums;		// one for projected lights, usually six for point lights
	shadowFrustum_t			shadowFrustums[6];

	int						viewCount;				// if == tr.viewCount, the light is on the viewDef->viewLights list
	struct viewLight_t* viewLight;

	idInteraction* firstInteraction;		// doubly linked list
	idInteraction* lastInteraction;

	idBounds				globalLightBounds;

	struct doublePortal_s* foggedPortals;
};


class idRenderEntityLocal : public idRenderEntity {
public:
	idRenderEntityLocal();

	virtual void			FreeRenderEntity();
	virtual void			UpdateRenderEntity(const renderEntity_t* re, bool forceUpdate = false);
	virtual void			GetRenderEntity(renderEntity_t* re);
	virtual void			ForceUpdate();
	virtual int				GetIndex();

	// overlays are extra polygons that deform with animating models for blood and damage marks
	virtual void			ProjectOverlay(const idPlane localTextureAxis[2], const idMaterial* material);
	virtual void			RemoveDecals();

	renderEntity_t			parms;

	float					modelMatrix[16];		// this is just a rearrangement of parms.axis and parms.origin

	idRenderWorldLocal* world;
	int						index;					// in world entityDefs

	int						lastModifiedFrameNum;	// to determine if it is constantly changing,
													// and should go in the dynamic frame memory, or kept
													// in the cached memory
	bool					archived;				// for demo writing

	idRenderModel* dynamicModel;			// if parms.model->IsDynamicModel(), this is the generated data
	int						dynamicModelFrameCount;	// continuously animating dynamic models will recreate
													// dynamicModel if this doesn't == tr.viewCount
	idRenderModel* cachedDynamicModel;

	idBounds				referenceBounds;		// the local bounds used to place entityRefs, either from parms or a model

	// a viewEntity_t is created whenever a idRenderEntityLocal is considered for inclusion
	// in a given view, even if it turns out to not be visible
	int						viewCount;				// if tr.viewCount == viewCount, viewEntity is valid,
													// but the entity may still be off screen
	struct viewEntity_s* viewEntity;				// in frame temporary memory

	int						visibleCount;
	// if tr.viewCount == visibleCount, at least one ambient
	// surface has actually been added by R_AddAmbientDrawsurfs
	// note that an entity could still be in the view frustum and not be visible due
	// to portal passing

	idRenderModelDecal* decals;					// chain of decals that have been projected on this model
	idRenderModelOverlay* overlay;				// blood overlays on animated models

	idInteraction* firstInteraction;		// doubly linked list
	idInteraction* lastInteraction;

	bool					needsPortalSky;
};

