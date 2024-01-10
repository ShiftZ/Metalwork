#pragma once

#include "RigidWorld.h"

inline Name anchor_name = "anchor";
inline Name shackle_name = "shackle";
inline Name rope_name = "rope";
inline Name weapon_name = "weapon";

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
	float length;

public:
	using Weapon::Weapon;
	void AttachAnchor(Anchor* anchor);
	void SetPosition(vec2 position) override;
	void LoadModel(Json::Value& jmodel) override;
	void SetOwner(Vessel* vessel) override;
};

class Anchor : public RigidObject
{
public:
	using RigidObject::RigidObject;
};
