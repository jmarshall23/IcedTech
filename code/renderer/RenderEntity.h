// RenderEntity.h
//

struct renderLight_t;

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
	virtual void			UpdateRenderEntity(const renderEntity_t* re, bool forceUpdate = false) = 0;
	virtual void			GetRenderEntity(renderEntity_t* re) = 0;
	virtual void			ForceUpdate() = 0;
	virtual int				GetIndex() = 0;

	// overlays are extra polygons that deform with animating models for blood and damage marks
	virtual void			ProjectOverlay(const idPlane localTextureAxis[2], const idMaterial* material) = 0;
	virtual void			RemoveDecals() = 0;
};
