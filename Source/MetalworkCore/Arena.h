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
	static bool BeginContact(Contact* contact, struct cpSpace* space, Arena* arena);
	static void EndContact(Contact* contact, struct cpSpace* space, Arena* arena);
	static bool PreSolve(Contact* contact, struct cpSpace* space, Arena* arena);
	static void PostSolve(Contact* contact, struct cpSpace* space, Arena* arena);

	friend class B2ContactListener;
};