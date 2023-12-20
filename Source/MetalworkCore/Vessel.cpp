#pragma warning(disable:4458)

#include "Vessel.h"
#include "Arena.h"
#include "CoreInterface.h"

Vessel::Vessel(Arena* arena, Name model_name)
{
	LoadModel(arena->rigid_world.get(), GetJson(model_name), "vehicle");
}

void Vessel::AttachWeapon(shared_ptr<Weapon> weapon)
{
	root->JoinRevolute(weapon->root, {0, -2}, -weapon->root_shift);
	this->weapon = move(weapon);
}

void Vessel::SetPosition(vec2 position)
{
	RigidObject::SetPosition(position);
	if (weapon) weapon->SetPosition(position);
}

