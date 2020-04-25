// Common_network.h
//

extern "C" {
	#include "../external/enet/enet.h"
};

//
// rvmNetworkServerState_t
//
struct rvmNetworkServerState_t {
	rvmNetworkServerState_t();

	ENetAddress address;
	ENetHost* server;
};

/*
==================
rvmNetworkServerState_t::rvmNetworkServerState_t
==================
*/
ID_INLINE rvmNetworkServerState_t::rvmNetworkServerState_t() {
	server = NULL;
}

//
// rvmNetworkClient_t
//
struct rvmNetworkClient_t {
	rvmNetworkClient_t();

	ENetHost* client;
	ENetAddress address;
	ENetPeer* peer;
	netadrtype_t addrType;
};

/*
==================
rvmNetworkClient_t::rvmNetworkClient_t
==================
*/
ID_INLINE rvmNetworkClient_t::rvmNetworkClient_t() {
	client = NULL;
	peer = NULL;
	addrType = NA_BAD;
}

