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
	void SetPosition(Vec2 position);
	void SetPlayerInput(Vec2 move_in);
};