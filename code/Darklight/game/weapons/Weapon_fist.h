// Weapon_fist.h
//

#pragma once

class rvmWeaponFist : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponFist);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();
private:
	const char *			GetFireAnim();
	bool side;
};