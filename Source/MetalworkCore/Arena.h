#pragma once

#include "RigidWorld.h"
#include "Contact.h"

class Arena
{
public:
	struct PlayerInput
	{
		vec2f move = {0, 0};
	};

	struct StepInputs
	{
		vector<flat_map<int, PlayerInput>> clean, dirty;
	};

public:
	vector<class Vessel*> vessels;
	unique_ptr<RigidWorld> rigid_world;
	int step = 0;
	mutex step_mtx;

public:
	Arena(unique_ptr<RigidWorld> rigid_world);
	void Start();
	void Step(StepInputs inputs);

	static METALWORKCORE_API unique_ptr<RigidWorld> MakeWorld();

private:
	struct Snapped
	{
		Body* body;
		vector<pair<Vec2, Vec2>> impulses;
	};

	vector<Snapped> snapped_bodies;

private:
	bool BeginContact(Contact* contact);
	void EndContact(Contact* contact);
	bool PreSolve(Contact* contact, const void* data);
	void PostSolve(Contact* contact, const void* data);

	friend class B2ContactListener;
};