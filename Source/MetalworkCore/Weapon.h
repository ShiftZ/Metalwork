#pragma once

#include "RigidWorld.h"

inline Name anchor_name = "anchor";
inline Name shackle_name = "shackle";
inline Name rope_name = "rope";
inline Name weapon_name = "weapon";

class Weapon : public RigidObject
{
public:
	using RigidObject::RigidObject;
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
};

class Anchor : public RigidObject
{
public:
	using RigidObject::RigidObject;
};
