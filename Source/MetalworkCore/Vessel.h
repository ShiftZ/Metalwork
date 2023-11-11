#pragma once

#include "Weapon.h"

class Vessel
{
public:
	unique_ptr<class RigidObject> body;
	shared_ptr<Weapon> weapon;
	class VehicleActor* actor = nullptr;

public:
	Vessel(class Arena* arena, string_view model_name);
	void AttachWeapon(shared_ptr<Weapon> weapon);
	void SetPosition(vec2 position);
};

class VehicleActor
{
};