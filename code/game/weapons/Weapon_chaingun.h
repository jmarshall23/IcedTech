// Weapon_chaingun.h
//

#pragma once

class rvmWeaponChainGun : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponChainGun);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();

	virtual bool			HasWaitSignal(void) override;
private:
	void		UpdateBarrel();
	void		SpinUp();
	void		SpinDown();
private:
	idAnimatedEntity	*world_model;
	jointHandle_t		world_barrel_joint;
	jointHandle_t		barrel_joint;
	float		barrel_angle;
	float		current_rate;
	float		start_rate;
	float		end_rate;
	float		spin_start;
	float		spin_end;
	float		spread;
	int			numSkipFrames;

	const idSoundShader		*snd_windup;
	const idSoundShader		*snd_winddown;
};