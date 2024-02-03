#pragma once

#include "RigidWorld.h"
#include "Weapon.h"

class Vessel : public RigidObject
{
public:
	Weapon* weapon = nullptr;
	Vec2 desired_accel = {0, 0};
	int player;

public:
	using RigidObject::RigidObject;
	Vessel(int player, Name name) : RigidObject(name), player(player) {}
	void AttachWeapon(Weapon* weapon);
	void SetPosition(Vec2 position);
	void SetPlayerInput(Vec2 move_in);
};