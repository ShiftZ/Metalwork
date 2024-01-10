#pragma warning(disable:4458)

#include "Weapon.h"
#include "Vessel.h"

void Weapon::SetOwner(Vessel* vessel)
{
	this->vessel = vessel;
}

void Chain::AttachAnchor(Anchor* anchor)
{
	Body* shackle = FindPart(shackle_name);

	anchor->SetPosition({0, -length});

	shared_ptr<Joint> shackle_joint = world->CreateRevoluteJoint(shackle, anchor->root, {0, -length});
	AddJoint(move(shackle_joint), shackle_name);

	this->anchor = anchor;
}

void Chain::SetPosition(vec2 position)
{
	RigidObject::SetPosition(position);
	if (anchor) anchor->SetPosition(position - vec2{0, length});
}

void Chain::LoadModel(Json::Value& jmodel)
{
	RigidObject::LoadModel(jmodel);
	length = ::length(RemovePart(anchor_name)->GetPosition());
}

void Chain::SetOwner(Vessel* vessel)
{
	Weapon::SetOwner(vessel);

	shared_ptr<Joint> rope_joint = world->CreateDistantJoint(vessel->root, anchor->root, length, vec2{0, 0}, vec2{0, -length});
	AddJoint(move(rope_joint), rope_name);
}
