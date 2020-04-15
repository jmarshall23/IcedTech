// ClientMoveable.h
//

#ifndef __GAME_CLIENT_MOVEABLE_H__
#define __GAME_CLIENT_MOVEABLE_H__

class rvClientMoveable : public rvClientEntity {
public:

	CLASS_PROTOTYPE( rvClientMoveable );

	rvClientMoveable ( void );
	virtual ~rvClientMoveable ( void );
	
	virtual void			Spawn			( void );
	virtual void			Think			( void );
	virtual idPhysics*		GetPhysics		( void ) const;	
	virtual bool			Collide			( const trace_t &collision, const idVec3 &velocity );
	
	idRenderEntity*			GetRenderEntity	( void );

	void					Save			( idSaveGame *savefile ) const;
	void					Restore			( idRestoreGame *savefile );

	bool					SetCustomShader(const char* shaderName);
	
	static void				SpawnClientMoveables ( idEntity* ent, const char *type, idList<rvClientMoveable *>* list );

	virtual void			FreeEntityDef	( void );

	void					SetOwner		( idEntity* ent );

	void					SetOrigin		( const idVec3& origin );
	void					SetAxis			( const idMat3& axis );
protected:
	idRenderEntity			*renderEntity;

//	rvClientEffectPtr		trailEffect;
//	float					trailAttenuateSpeed;
		
	idPhysics_RigidBody		physicsObj;
	
	int						bounceSoundTime;
	const idSoundShader*	bounceSoundShader;
	bool					mPlayBounceSoundOnce;
	bool					mHasBounced;

	idInterpolate<float>	scale;
	
private:
	
	void					Event_FadeOut			( int duration );
	void					Event_ClearDepthHack	( void );
};

ID_INLINE idRenderEntity* rvClientMoveable::GetRenderEntity ( void ) {
	return renderEntity;
}

extern const idEventDef CL_FadeOut;
extern const idEventDef CL_ClearDepthHack;

#define SIMPLE_TRI_NAME		"simpletri"

extern idVec3 simpleTri[3];


#endif // __GAME_CLIENT_MOVEABLE_H__
