#pragma once

#include <memory>
#include "RigidWorld.h"
#include "Weapon.h"

using namespace std;

class Vehicle
{
public:
	unique_ptr<RigidObject> body;
	shared_ptr<Weapon> weapon;
	class VehicleActor* actor = nullptr;

public:
	Vehicle(class Arena* arena, string_view model_name);
	void AttachWeapon(shared_ptr<Weapon> weapon);
	void SetPosition(vec2 position);
};

class VehicleActor
{
};