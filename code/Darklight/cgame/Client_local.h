// Client_local.h
//

#include "Client_PlayerView.h"

//
// rvmClientLocal
//
class rvmClientLocal {
public:
	void Init(void);
	void BeginLevel(void);
	void MapShutdown(void);
	void RenderPlayerView(int clientNum);
private:
	idPlayerView*					playerView;
};

extern rvmClientLocal clientLocal;