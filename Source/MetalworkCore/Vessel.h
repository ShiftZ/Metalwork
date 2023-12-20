#pragma once

#include "RigidWorld.h"
#include "Weapon.h"

class Vessel : public RigidObject
{
public:
	shared_ptr<Weapon> weapon;

public:
	Vessel(class Arena* arena, Name model_name);
	void AttachWeapon(shared_ptr<Weapon> weapon);
	void SetPosition(vec2 position);
};