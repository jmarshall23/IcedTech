// ClientEffects.h
//

//
// rvmClientEffect
//
class rvmClientEffect : public idEntity {
public:
					~rvmClientEffect();
	virtual void	Spawn(void);
protected:
	virtual void	Think(void);
	void			AddClientModel(const idDict& clientEntityDef, idVec3 origin, idMat3 axis, idVec3 velocity, const char* shaderName);

	idList<rvClientModel*> clientEntityDebris;

	int				deleteFrame;
};

//
// rvmClientEffect_debris
//
class rvmClientEffect_debris : public rvmClientEffect {

					~rvmClientEffect_debris();
	CLASS_PROTOTYPE(rvmClientEffect_debris);
public:
	virtual void	Spawn(void);
	virtual void	Think(void);

	void			LaunchEffect(const idDeclEntityDef** debrisArray, int debrisArraySize, idVec3 origin, idMat3 axis, const char* shaderName);
private:
	
};