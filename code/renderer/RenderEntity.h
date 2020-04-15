// RenderEntity.h
//

struct renderLight_t;
class idSoundEmitter;

//
// idRenderLight
//
class idRenderLight {
public:
	virtual					~idRenderLight() {}

	virtual void			FreeRenderLight() = 0;
	virtual void			UpdateRenderLight(bool forceUpdate = false) = 0;
	virtual void			ForceUpdate() = 0;
	virtual int				GetIndex() = 0;

	virtual void			SetLightChannel(int lightChannel, bool enabled) = 0;
	virtual bool			HasLightChannel(int lightChannel) = 0;
	virtual void			ClearLightChannel(void) = 0;

	virtual void			SetName(const char* name) = 0;
	virtual const char*		GetName(void) = 0;

	virtual idMat3			GetAxis(void) = 0;
	virtual void			SetAxis(idMat3 axis) = 0;
	
	virtual idVec3			GetOrigin(void) = 0;
	virtual void			SetOrigin(idVec3 origin) = 0;

	virtual int				GetUniqueLightId(void) = 0;
	virtual void			SetUniqueLightId(int uniqueLightId) = 0;

	virtual bool			GetDynamicShadows(void) = 0;
	virtual void			SetDynamicShadows(bool dynamicShadows) = 0;

	virtual int				GetSurpressLightInViewID(void) = 0;
	virtual void			SetSurpressLightInViewID(int suppressLightInViewID) = 0;

	virtual int				GetAllowLightInViewID(void) = 0;
	virtual void			SetAllowLightInViewID(int allowLightInViewID) = 0;

	virtual bool			GetNoShadows(void) = 0;
	virtual void			SetNoShadows(bool noShadows) = 0;

	virtual bool			GetNoSpecular(void) = 0;
	virtual void			SetNoSpecular(bool noSpecular) = 0;

	virtual bool			GetPointLight(void) = 0;
	virtual void			SetPointLight(bool pointLight) = 0;

	virtual bool			GetParallelLight(void) = 0;
	virtual void			SetParallelLight(bool parallel) = 0;

	virtual idVec3			GetLightRadius(void) = 0;
	virtual void			SetLightRadius(idVec3 lightRadius) = 0;

	virtual idVec3			GetLightCenter(void) = 0;
	virtual void			SetLightCenter(idVec3 lightCenter) = 0;

	virtual bool			GetAmbientLight(void) = 0;
	virtual void			SetAmbientLight(bool ambientLight) = 0;

	virtual idVec3			GetTarget(void) = 0;
	virtual void			SetTarget(idVec3 target) = 0;

	virtual idVec3			GetRight(void) = 0;
	virtual void			SetRight(idVec3 right) = 0;

	virtual idVec3			GetUp(void) = 0;
	virtual void			SetUp(idVec3 up) = 0;

	virtual idVec3			GetStart(void) = 0;
	virtual void			SetStart(idVec3 start) = 0;

	virtual idVec3			GetEnd(void) = 0;
	virtual void			SetEnd(idVec3 end) = 0;

	virtual int				GetLightId(void) = 0;
	virtual void			SetLightId(int lightId) = 0;

	virtual const idMaterial* GetShader(void) = 0;
	virtual void			  SetShader(const idMaterial* material) = 0;

	virtual float			GetShaderParam(int index) = 0;
	virtual void			SetShaderParam(int index, float value) = 0;

	virtual idSoundEmitter* GetReferenceSound(void) = 0;
	virtual void			SetReferenceSound(idSoundEmitter* referenceSound) = 0;
};


//
// idRenderEntity
//
class idRenderEntity {
public:
	virtual					~idRenderEntity() {}

	virtual void			FreeRenderEntity() = 0;
	virtual void			UpdateRenderEntity(bool forceUpdate = false) = 0;
	virtual void			ForceUpdate() = 0;
	virtual int				GetIndex() = 0;

	virtual void			ClearParms(void) = 0;

	virtual void			ImportParms(const idRenderEntity* entity) = 0;

	virtual	bool			HasValidJoints() const = 0;
	virtual	bool			HasLightChannel(int lightChannel) = 0;
	virtual	void			SetLightChannel(int lightChannel, bool enabled) = 0;

	virtual idRenderModel*  GetRenderModel(void) = 0;
	virtual void			SetRenderModel(idRenderModel* model) = 0;
	virtual void			ClearLightChannel(void) = 0;

	virtual renderClassWorldType_t GetRenderClassType(void) = 0;
	virtual void			SetRenderClassType(renderClassWorldType_t classType) = 0;

	virtual int				GetEntityNum(void) = 0;
	virtual void			SetEntityNum(int entityNum) = 0;

	virtual int				GetBodyId(void) = 0;
	virtual void			SetBodyId(int bodyId) = 0;

	virtual int				GetFrameNum(void) = 0;
	virtual void			SetFrameNum(int frameNum) = 0;

	virtual bool			GetNoShadow(void) = 0;
	virtual void			SetNoShadow(bool noShadow) = 0;

	virtual bool			GetHasDynamicShadows(void) = 0;
	virtual void			SetDynamicShadows(bool dynamicShadows) = 0;

	virtual bool			GetSkipEntityViewCulling(void) = 0;
	virtual void			SetSkipEntityViewCulling(bool skipEntityViewCulling) = 0;

	virtual bool			GetForceVirtualTextureHighQuality(void) = 0;
	virtual void			SetForceVirtualtextureHighQuality(bool forceVirtualTextureHighQuality) = 0;

	virtual idBounds		GetBounds(void) = 0;
	virtual void			SetBounds(idBounds bounds) = 0;

	virtual deferredEntityCallback_t GetCallback(void) = 0;
	virtual void			SetCallback(deferredEntityCallback_t callback) = 0;

	virtual void*			GetCallbackData(void) = 0;
	virtual void			SetCallbackData(void* callbackData) = 0;

	virtual int				GetSuppressSurfaceInViewID(void) = 0;
	virtual void			SetSuppressSurfaceInViewID(int suppressSurfaceInViewID) = 0;

	virtual int				GetSuppressShadowInViewID(void) = 0;
	virtual void			SetSuppressShadowInViewID(int suppressShadowInViewID) = 0;

	virtual int				GetSuppressShadowInLightID(void) = 0;
	virtual void			SetSuppressShadowInLightID(int suppressShadowInLightID) = 0;

	virtual int				GetAllowSurfaceInViewID(void) = 0;
	virtual void			SetAllowSurfaceInViewID(int suppressAllowInViewID) = 0;

	virtual idVec3			GetOrigin(void) = 0;
	virtual void			SetOrigin(idVec3 origin) = 0;

	virtual idMat3			GetAxis(void) = 0;
	virtual void			SetAxis(idMat3 axis) = 0;

	virtual const idMaterial* GetCustomShader(void) = 0;
	virtual void			SetCustomShader(const idMaterial* shader) = 0;

	virtual const idMaterial* GetReferenceShader(void) = 0;
	virtual void			SetReferenceShader(const idMaterial* referenceShader) = 0;

	virtual const idDeclSkin* GetCustomSkin(void) = 0;
	virtual void			SetCustomSkin(const idDeclSkin* customSkin) = 0;

	virtual const idSoundEmitter* GetReferenceSound(void) = 0;
	virtual void			SetReferenceSound(idSoundEmitter* referenceSound) = 0;

	virtual float			GetShaderParms(int index) = 0;
	virtual void			SetShaderParms(int index, float value) = 0;

	virtual class idUserInterface* GetGui(int index) = 0;
	virtual void			SetGui(int index, class idUserInterface* gui) = 0;

	virtual bool			GetHidden(void) = 0;
	virtual void			SetHidden(bool isHidden) = 0;

	virtual struct renderView_t* GetRemoteRenderView(void) = 0;
	virtual void			SetRemoteRenderView(struct renderView_t* remoteRenderView) = 0;

	virtual int				GetNumjoints(void) = 0;
	virtual void			SetNumJoints(int numJoints) = 0;

	virtual idJointMat*		GetJoints(void) = 0;
	virtual void			SetJoints(idJointMat* joints) = 0;

	virtual float			GetModelDepthHack(void) = 0;
	virtual void			SetModelDepthHack(float modelDepthHack) = 0;

	virtual bool			GetNoSelfShadow(void) = 0;
	virtual void			SetNoSelfShadow(bool noSelfShadow) = 0;

	virtual bool			GetNoDynamicInteractions(void) = 0;
	virtual void			SetNoDynamicInteractions(bool noDynamicInteractions) = 0;

	virtual bool			GetWeaponDepthHack(void) = 0;
	virtual void			SetWeaponDepthHack(bool weaponDepthHack) = 0;

	virtual int				GetForceUpdate(void) = 0;
	virtual void			SetForceUpdate(int forceUpdate) = 0;

	virtual int				GetTimeGroup(void) = 0;
	virtual void			SetTimeGroup(int timeGroup) = 0;

	virtual int				GetXrayIndex(void) = 0;
	virtual void			SetXrayIndex(int xrayIndex) = 0;
};
