// Weapon_pistol.h
//

#pragma once

class rvmWeaponPistol : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponPistol);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();

	virtual bool			HasWaitSignal(void) override;
private:
	float					next_attack;
	float					spread;

	const idSoundShader		*snd_lowammo;
};