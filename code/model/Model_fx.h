// Model_fx.h
//

#define PARTICLE_SIM_IDEN			"ParticleVersion"
#define PARTICLE_SIM_VERSION		"2"

//
// rvmEffectParticleState_t
//
enum rvmEffectParticleState_t {
	PARTICLE_NOTSET = 0,
	PARTICLE_STATE_ALIVE,
	PARTICLE_STATE_DEAD,
	PARTICLE_STATE_UNBORN
};

//
// rvmEffectPoint_t
//
struct rvmEffectPoint_t {
	rvmEffectPoint_t();

	idVec3 xyz;
	rvmEffectParticleState_t state;
	float normalized_time;
	float size;
};

/*
======================
rvmEffectPoint_t::rvmEffectPoint_t
======================
*/
ID_INLINE rvmEffectPoint_t::rvmEffectPoint_t() {
	state = PARTICLE_NOTSET;
	xyz.Zero();
	normalized_time = 0;
	size = 0;
}

//
// rvmEffectSimFrame_t
//
struct rvmEffectSimFrame_t {
	idList<rvmEffectPoint_t> points;
};

//
// rvmEffectSimulation_t
//
struct rvmEffectSimulation_t {	
	idList<rvmEffectSimFrame_t> frames;
};

//
// rvmEffect_t
//
struct rvmEffect_t {
	rvmEffect_t();

	idStr name;
	const idMaterial* mtr;
	rvmEffectSimulation_t simulation;
	float sim_scale;
};

ID_INLINE rvmEffect_t::rvmEffect_t() {
	sim_scale = 1.0f;
}