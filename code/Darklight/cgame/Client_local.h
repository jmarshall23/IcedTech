// Client_local.h
//

#include "Client_PlayerView.h"
#include "entities/ClientEntity.h"
#include "entities/ClientMoveable.h"
#include "entities/ClientModel.h"
#include "Client_Effects.h"

#define	CENTITYNUM_BITS			12
#define	MAX_CENTITIES			(1<<CENTITYNUM_BITS)

//
// rvmClientPhysicsJobParams_t
//
struct rvmClientPhysicsJobParams_t {
	int startClientEntity;
	int numClientEntities;
	int clientThreadId;
};

//
// rvmClientGameLocal
//
class rvmClientGameLocal {
	friend class rvClientModel;
	friend class rvClientEntity;
	friend class rvClientPhysics;
	friend class idGameLocal;
public:
	void Init(void);
	void BeginLevel(void);
	void MapShutdown(void);
	void RenderPlayerView(int clientNum);
public:
	void						RegisterClientEntity(rvClientEntity* cent);
	void						UnregisterClientEntity(rvClientEntity* cent);
	bool						SpawnClientEntityDef(const idDict& args, rvClientEntity** cent, bool setDefaults, const char* spawn);
protected:
	static void					ClientEntityJob_t(rvmClientPhysicsJobParams_t* params);

	idList<rvClientEntity*>		activeClientEntities;

	idParallelJobList*			clientPhysicsJob;
	idList<rvClientEntity*>		clientEntityThreadWork;

	int							clientSpawnCount;
	idPlayerView*				playerView;
	rvClientEntity*				clientEntities[MAX_CENTITIES];	// index to client entities
	int							clientSpawnIds[MAX_CENTITIES];	// for use in idClientEntityPtr
	idLinkList<rvClientEntity>	clientSpawnedEntities;			// all client side entities
	int							num_clientEntities;				// current number of client entities
	int							firstFreeClientIndex;			// first free index in the client entities array
};

extern rvmClientGameLocal clientLocal;