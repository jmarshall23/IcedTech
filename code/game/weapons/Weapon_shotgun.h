// Weapon_shotgun.h
//

#pragma once

class rvmWeaponShotgun : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponShotgun);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();

	virtual bool			HasWaitSignal(void) override;
private:
	float		next_attack;
	float		spread;

	const idSoundShader		*snd_lowammo;
};