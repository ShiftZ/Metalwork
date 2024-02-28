#pragma warning(disable:4458)

#include "Weapon.h"
#include "Vessel.h"
#include "Joint.h"
#include "Body.h"
#include "RigidWorld.h"

void Weapon::SetOwner(Vessel* vessel)
{
	this->vessel = vessel;
}

void Chain::AttachAnchor(Anchor* anchor)
{
	Body* shackle = FindPart(shackle_name);

	anchor->SetPosition({0, -length});

	shackle_joint = world->CreateRevoluteJoint(shackle, anchor->root, {0, -length});
	shackle_joint->name = shackle_name;

	this->anchor = anchor;
}

void Chain::SetPosition(Vec2 position)
{
	RigidObject::SetPosition(position);
	if (anchor) anchor->SetPosition(position - Vec2{0, length});
}

void Chain::LoadModel(Json::Value& jmodel)
{
	RigidObject::LoadModel(jmodel);
	length = Length(RemovePart(anchor_name)->GetPosition());
}

void Chain::SetOwner(Vessel* vessel)
{
	Weapon::SetOwner(vessel);
	rope_joint = world->CreateDistantJoint(vessel->root, anchor->root, length, Vec2{0, 0}, Vec2{0, -length});
}
