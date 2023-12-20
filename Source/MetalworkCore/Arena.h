#pragma once

#include "RigidWorld.h"

class Arena
{
public:
	struct PlayerInput
	{
		vec2 move = nullvec;
	};

	struct StepInputs
	{
		vector<flat_map<int, PlayerInput>> clean, dirty;
	};

public:
	vector<shared_ptr<class Vessel>> vessels;
	unique_ptr<RigidWorld> rigid_world;
	int step = 0;
	mutex step_mtx;

public:
	Arena(unique_ptr<RigidWorld>);
	void Start();
	void Step(StepInputs inputs);

	span<shared_ptr<Vessel>> Vessels() { return vessels; }

	static METALWORKCORE_API unique_ptr<RigidWorld> MakeWorld();
};