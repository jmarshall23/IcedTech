// Weapon_flashlight.h
//

#pragma once

class rvmWeaponFlashlight : public rvmWeaponObject {
public:
	CLASS_PROTOTYPE(rvmWeaponFlashlight);

	virtual void			Init(idWeapon *weapon);

	virtual void			Raise();
	virtual void			Idle();
	virtual void			Fire();
	virtual void			Lower();
	virtual void			Reload();
private:
	void					UpdateSkin(void);
	void					UpdateLightIntensity(void);

	bool					on;
	float					intensity;
	idStr					skin_on;
	idStr					skin_on_invis;
	idStr					skin_off;
	idStr					skin_off_invis;
};
