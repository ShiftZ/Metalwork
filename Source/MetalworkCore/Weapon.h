#pragma once

#include "RigidWorld.h"

inline const Name anchor_name = "anchor";
inline const Name shackle_name = "shackle";
inline const Name rope_name = "rope";
inline const Name weapon_name = "weapon";

class Weapon : public RigidObject
{
public:
	class Vessel* vessel = nullptr;

public:
	using RigidObject::RigidObject;
	virtual void SetOwner(Vessel* vessel);
};

class Chain : public Weapon
{
public:
	class Anchor* anchor = nullptr;
	shared_ptr<Joint> rope_joint;
	shared_ptr<Joint> shackle_joint;
	Float length;

public:
	using Weapon::Weapon;
	void AttachAnchor(Anchor* anchor);
	void SetPosition(Vec2 position) override;
	void LoadModel(Json::Value& jmodel) override;
	void SetOwner(Vessel* vessel) override;
};

class Anchor : public RigidObject
{
public:
	using RigidObject::RigidObject;
};
