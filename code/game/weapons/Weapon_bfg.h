// Weapon_bfg.h
//

#pragma once

class rvmWeaponBFG : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponBFG);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();
private:
	void					OverCharge();

	float					next_attack;
	float					spread;

	float					fuse_start;
	float					fuse_end;
	float					powerLevel;
	float					fire_time;
};