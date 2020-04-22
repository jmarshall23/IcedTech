// RenderEntity_local.h
//

//
// idRenderLightParms
//
struct idRenderLightParms {
	idRenderLightParms();

	idStr name;	

	rvmLightRenderType_t	lightRenderType;

	bool					isEnabled;

	renderClassWorldType_t	classType;

	idMat3					axis;				// rotation vectors, must be unit length
	idVec3					origin;

	int						lightChannel;

	int						uniqueLightId;		// light id that is unique to this light

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
	idVec3					lightColor;
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

	idImage*				iesTexture;

	// Dmap will generate an optimized shadow volume named _prelight_<lightName>
	// for the light against all the _area* models in the map.  The renderer will
	// ignore this value if the light has been moved after initial creation
	idRenderModel* prelightModel;

	// muzzle flash lights will not cast shadows from player and weapon world models
	int						lightId;


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

	virtual rvmLightRenderType_t GetLightRenderType(void) { return parms.lightRenderType; }
	virtual void			SetLightRenderType(rvmLightRenderType_t lightRenderType) {
		parms.lightRenderType = lightRenderType;
		parmsDirty = true;
	}

	virtual renderClassWorldType_t GetRenderClassType() { return parms.classType; }
	virtual void			SetRenderClassType(renderClassWorldType_t type) { parmsDirty = true; parms.classType = type; }

	virtual bool			IsEnabled(void) { return parms.isEnabled; }
	virtual void			SetEnabled(bool isEnabled) { parms.isEnabled = isEnabled; parmsDirty = true; }

	virtual idImage*		GetIESTexture(void) { return parms.iesTexture; }
	virtual void			SetIESTexture(idImage* image) { parms.iesTexture = image; parmsDirty = true; }

	virtual idVec3			GetLightColor(void) { return parms.lightColor; }
	virtual void			SetLightColor(idVec3 lightColor) {
		parms.lightColor = lightColor;
		parmsDirty = true;
	}

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


	// Shadow Matrixes 
	idRenderMatrix			shadowMatrix[6];

	struct doublePortal_s* foggedPortals;
};



//
// idRenderEntityParms
//
struct idRenderEntityParms {
	idRenderModel* hModel;				// this can only be null if callback is set

	renderClassWorldType_t	classType;

	int						lightChannel;

	int						entityNum;
	int						bodyId;

	int						frameNum;

	bool					isHidden;

	bool					hasDynamicShadows;

	bool					skipEntityViewCulling;

	// This disables mipmapping on this render entity.
	bool					forceVirtualTextureHighQuality;

	// Entities that are expensive to generate, like skeletal models, can be
	// deferred until their bounds are found to be in view, in the frustum
	// of a shadowing light that is in view, or contacted by a trace / overlay test.
	// This is also used to do visual cueing on items in the view
	// The renderView may be NULL if the callback is being issued for a non-view related
	// source.
	// The callback function should clear renderEntity->callback if it doesn't
	// want to be called again next time the entity is referenced (ie, if the
	// callback has now made the entity valid until the next updateEntity)
	idBounds				bounds;					// only needs to be set for deferred models and md5s
	deferredEntityCallback_t	callback;

	void* callbackData;			// used for whatever the callback wants

	// player bodies and possibly player shadows should be suppressed in views from
	// that player's eyes, but will show up in mirrors and other subviews
	// security cameras could suppress their model in their subviews if we add a way
	// of specifying a view number for a remoteRenderMap view
	int						suppressSurfaceInViewID;
	int						suppressShadowInViewID;

	// world models for the player and weapons will not cast shadows from view weapon
	// muzzle flashes
	int						suppressShadowInLightID;

	// if non-zero, the surface and shadow (if it casts one)
	// will only show up in the specific view, ie: player weapons
	int						allowSurfaceInViewID;

	// positioning
	// axis rotation vectors must be unit length for many
	// R_LocalToGlobal functions to work, so don't scale models!
	// axis vectors are [0] = forward, [1] = left, [2] = up
	idVec3					origin;
	idMat3					axis;

	// texturing
	const idMaterial* customShader;			// if non-0, all surfaces will use this
	const idMaterial* referenceShader;		// used so flares can reference the proper light shader
	const idDeclSkin* customSkin;				// 0 for no remappings
	class idSoundEmitter* referenceSound;			// for shader sound tables, allowing effects to vary with sounds
	float					shaderParms[MAX_ENTITY_SHADER_PARMS];	// can be used in any way by shader or model generation

	// networking: see WriteGUIToSnapshot / ReadGUIFromSnapshot
	class idUserInterface* gui[MAX_RENDERENTITY_GUI];

	struct renderView_t* remoteRenderView;		// any remote camera surfaces will use this

	int						numJoints;
	idJointMat* joints;					// array of joints that will modify vertices.
													// NULL if non-deformable model.  NOT freed by renderer

	float					modelDepthHack;			// squash depth range so particle effects don't clip into walls

	// options to override surface shader flags (replace with material parameters?)
	bool					noSelfShadow;			// cast shadows onto other objects,but not self
	bool					noShadow;				// no shadow at all

	bool					noDynamicInteractions;	// don't create any light / shadow interactions after
													// the level load is completed.  This is a performance hack
													// for the gigantic outdoor meshes in the monorail map, so
													// all the lights in the moving monorail don't touch the meshes

	bool					weaponDepthHack;		// squash depth range so view weapons don't poke into walls
													// this automatically implies noShadow
	int						forceUpdate;			// force an update (NOTE: not a bool to keep this struct a multiple of 4 bytes)
	int						timeGroup;
	int						xrayIndex;
};

class idRenderEntityLocal : public idRenderEntity {
public:
	idRenderEntityLocal();

	virtual void			FreeRenderEntity();
	virtual void			UpdateRenderEntity(bool forceUpdate = false);
	virtual void			ForceUpdate();
	virtual int				GetIndex();

	virtual	bool			HasValidJoints() const;
	virtual	bool			HasLightChannel(int lightChannel);
	virtual	void			SetLightChannel(int lightChannel, bool enabled);
	virtual void			ClearLightChannel(void) { parmsDirty = true; parms.lightChannel = 0; }

	virtual void			ClearParms(void) { memset(&parms, 0, sizeof(parms)); }

	virtual void			ImportParms(const idRenderEntity* entity) { parmsDirty = true;  parms = ((const idRenderEntityLocal*)entity)->parms; }

	virtual idRenderModel* GetRenderModel(void) { return parms.hModel; }
	virtual void			SetRenderModel(idRenderModel* model) { parmsDirty = true; modelChanged = true;  parms.hModel = model; }

	virtual renderClassWorldType_t GetRenderClassType(void) { return parms.classType; }
	virtual void			SetRenderClassType(renderClassWorldType_t classType) { parmsDirty = true; parms.classType = classType; }

	virtual bool			GetNoShadow(void) { return parms.noShadow; }
	virtual void			SetNoShadow(bool noShadow) { parms.noShadow = noShadow; }

	virtual int				GetEntityNum(void) { return parms.entityNum; }
	virtual void			SetEntityNum(int entityNum) { parmsDirty = true; parms.entityNum = entityNum; }

	virtual int				GetBodyId(void) { return parms.bodyId; }
	virtual void			SetBodyId(int bodyId) { parmsDirty = true; parms.bodyId = bodyId; }

	virtual int				GetFrameNum(void) { return parms.frameNum; }
	virtual void			SetFrameNum(int frameNum) { parmsDirty = true; parms.frameNum = frameNum; }

	virtual bool			GetHidden(void) { return parms.isHidden; }
	virtual void			SetHidden(bool isHidden) { parmsDirty = true; parms.isHidden = isHidden; }

	virtual bool			GetHasDynamicShadows(void) { return parms.hasDynamicShadows; }
	virtual void			SetDynamicShadows(bool dynamicShadows) { parmsDirty = true; parms.hasDynamicShadows = dynamicShadows; }

	virtual bool			GetSkipEntityViewCulling(void) { return parms.skipEntityViewCulling; }
	virtual void			SetSkipEntityViewCulling(bool skipEntityViewCulling) { parmsDirty = true; parms.skipEntityViewCulling = skipEntityViewCulling; }

	virtual bool			GetForceVirtualTextureHighQuality(void) { return parms.forceVirtualTextureHighQuality; }
	virtual void			SetForceVirtualtextureHighQuality(bool forceVirtualTextureHighQuality) { parmsDirty = true; parms.forceVirtualTextureHighQuality = forceVirtualTextureHighQuality; }

	virtual idBounds		GetBounds(void) { return parms.bounds; }
	virtual void			SetBounds(idBounds bounds) { parmsDirty = true; parms.bounds = bounds; }

	virtual deferredEntityCallback_t GetCallback(void) { return parms.callback; }
	virtual void			SetCallback(deferredEntityCallback_t callback) { parmsDirty = true; parms.callback = callback; }

	virtual void*			GetCallbackData(void) { return parms.callbackData; }
	virtual void			SetCallbackData(void* callbackData) { parmsDirty = true; parms.callbackData = callbackData; }

	virtual int				GetSuppressSurfaceInViewID(void) { return parms.suppressSurfaceInViewID; }
	virtual void			SetSuppressSurfaceInViewID(int suppressSurfaceInViewID) { parmsDirty = true; parms.suppressSurfaceInViewID = suppressSurfaceInViewID; }

	virtual int				GetSuppressShadowInViewID(void) { return parms.suppressShadowInViewID; }
	virtual void			SetSuppressShadowInViewID(int suppressShadowInViewID) { parmsDirty = true; parms.suppressShadowInViewID = suppressShadowInViewID; }

	virtual int				GetSuppressShadowInLightID(void) { return parms.suppressShadowInLightID; }
	virtual void			SetSuppressShadowInLightID(int suppressShadowInLightID) { parmsDirty = true; parms.suppressShadowInLightID = suppressShadowInLightID; }

	virtual int				GetAllowSurfaceInViewID(void) { return parms.allowSurfaceInViewID; }
	virtual void			SetAllowSurfaceInViewID(int suppressAllowInViewID) { parmsDirty = true; parms.allowSurfaceInViewID = suppressAllowInViewID; }

	virtual idVec3			GetOrigin(void) { return parms.origin; }
	virtual void			SetOrigin(idVec3 origin) { parmsDirty = true; parms.origin = origin; }

	virtual idMat3			GetAxis(void) { return parms.axis; }
	virtual void			SetAxis(idMat3 axis) { parmsDirty = true; parms.axis = axis; }

	virtual const idMaterial* GetCustomShader(void) { return parms.customShader; }
	virtual void			SetCustomShader(const idMaterial* shader) { parmsDirty = true; parms.customShader = shader; }

	virtual const idMaterial* GetReferenceShader(void) { return parms.referenceShader; }
	virtual void			SetReferenceShader(const idMaterial* referenceShader) { parmsDirty = true; parms.referenceShader = referenceShader; }

	virtual const idDeclSkin* GetCustomSkin(void) { return parms.customSkin; }
	virtual void			SetCustomSkin(const idDeclSkin* customSkin) { parmsDirty = true; parms.customSkin = customSkin; }

	virtual const idSoundEmitter* GetReferenceSound(void) { return parms.referenceSound; }
	virtual void			SetReferenceSound(idSoundEmitter* referenceSound) { parms.referenceSound = referenceSound; }

	virtual float			GetShaderParms(int index) { return parms.shaderParms[index]; }
	virtual void			SetShaderParms(int index, float value) { parmsDirty = true; parms.shaderParms[index] = value; }

	virtual class idUserInterface* GetGui(int index) { return parms.gui[index]; }
	virtual void			SetGui(int index, class idUserInterface* gui) { parmsDirty = true; parms.gui[index] = gui; }

	virtual struct renderView_t* GetRemoteRenderView(void) { return parms.remoteRenderView; }
	virtual void			SetRemoteRenderView(struct renderView_t* remoteRenderView) { parmsDirty = true; parms.remoteRenderView = remoteRenderView; }

	virtual int				GetNumjoints(void) { return parms.numJoints; }
	virtual void			SetNumJoints(int numJoints) { parmsDirty = true; parms.numJoints = numJoints; }

	virtual idJointMat* GetJoints(void) { return parms.joints; }
	virtual void			SetJoints(idJointMat* joints) { parmsDirty = true; parms.joints = joints; }

	virtual float			GetModelDepthHack(void) { return parms.modelDepthHack; }
	virtual void			SetModelDepthHack(float modelDepthHack) { parmsDirty = true; parms.modelDepthHack = modelDepthHack; }

	virtual bool			GetNoSelfShadow(void) { return parms.noSelfShadow; }
	virtual void			SetNoSelfShadow(bool noSelfShadow) { parmsDirty = true; parms.noSelfShadow = noSelfShadow; }

	virtual bool			GetNoDynamicInteractions(void) { return parms.noDynamicInteractions; }
	virtual void			SetNoDynamicInteractions(bool noDynamicInteractions) { parmsDirty = true; parms.noDynamicInteractions = noDynamicInteractions; }

	virtual bool			GetWeaponDepthHack(void) { return parms.weaponDepthHack; }
	virtual void			SetWeaponDepthHack(bool weaponDepthHack) { parmsDirty = true; parms.weaponDepthHack = weaponDepthHack; }

	virtual int				GetForceUpdate(void) { return parms.forceUpdate; }
	virtual void			SetForceUpdate(int forceUpdate) { parmsDirty = true; parms.forceUpdate = forceUpdate; }

	virtual int				GetTimeGroup(void) { return parms.timeGroup; }
	virtual void			SetTimeGroup(int timeGroup) { parmsDirty = true; parms.timeGroup = timeGroup; }

	virtual int				GetXrayIndex(void) { return parms.xrayIndex; }
	virtual void			SetXrayIndex(int xrayIndex) { parmsDirty = true; parms.xrayIndex = xrayIndex; }
public:
	bool					parmsDirty;
	bool					modelChanged;

	idRenderEntityParms			parms;

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

