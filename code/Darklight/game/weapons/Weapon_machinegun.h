// Weapon_machinegun.h
//

#pragma once

class rvmWeaponMachineGun : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponMachineGun);

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