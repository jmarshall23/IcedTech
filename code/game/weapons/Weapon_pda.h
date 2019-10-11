// Weapon_pda.h
//

#pragma once

class rvmWeaponPDA : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponPDA);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();
};