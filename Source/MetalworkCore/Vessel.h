#pragma once

#include "RigidWorld.h"
#include "Weapon.h"

class Vessel : public RigidObject
{
public:
	Weapon* weapon = nullptr;

public:
	using RigidObject::RigidObject;
	void AttachWeapon(Weapon* weapon);
	void SetPosition(vec2 position);
	void SetPlayerInput(vec2 move_in);
};