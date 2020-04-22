// Weapon_rocketlauncher.h
//

#pragma once

class rvmWeaponRocketLauncher : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponRocketLauncher);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();
private:
	void					UpdateSkin();

	float		next_attack;
	float		spread;
	idStr		skin_invisible;
};