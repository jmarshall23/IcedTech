// Model_fx.h
//

#define PARTICLE_SIM_IDEN			"ParticleVersion"
#define PARTICLE_SIM_VERSION		"1"

//
// rvmEffectSimFrame_t
//
struct rvmEffectSimFrame_t {
	idList<idVec3> points;
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
	idStr name;
	const idMaterial* mtr;
	rvmEffectSimulation_t simulation;
};