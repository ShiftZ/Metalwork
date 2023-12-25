#pragma warning(disable:4458)

#include "Vessel.h"
#include "Arena.h"

void Vessel::AttachWeapon(Weapon* weapon)
{
	root->JoinRevolute(weapon->root, {0, -2}, -weapon->root->offset);
	this->weapon = weapon;
}

void Vessel::SetPosition(vec2 position)
{
	RigidObject::SetPosition(position);
	if (weapon) weapon->SetPosition(position);
}

