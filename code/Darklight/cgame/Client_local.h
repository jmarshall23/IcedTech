// Client_local.h
//

#include "Client_PlayerView.h"

//
// rvmClientGameLocal
//
class rvmClientGameLocal {
public:
	void Init(void);
	void BeginLevel(void);
	void MapShutdown(void);
	void RenderPlayerView(int clientNum);
private:
	idPlayerView*					playerView;
};

extern rvmClientGameLocal clientLocal;