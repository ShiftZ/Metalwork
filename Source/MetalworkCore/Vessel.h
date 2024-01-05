#pragma once

#include "RigidWorld.h"
#include "Weapon.h"

class Vessel : public RigidObject
{
public:
	Weapon* weapon = nullptr;

	shared_ptr<Joint> root_joint;
	shared_ptr<Joint> head_joint;

public:
	using RigidObject::RigidObject;
	void AttachWeapon(Weapon* weapon);
	void SetPosition(vec2 position);
	void SetPlayerInput(vec2 move_in);
};