#pragma once

#include <memory>
#include <vector>
#include "flat_map.h"

#include "RigidWorld.h"

class Arena
{
public:
	struct PlayerInput
	{
		vec2i move = nullvec;
	};

	struct StepInputs
	{
		vector<flat_map<int, PlayerInput>> clean, dirty;
	};

public:
	vector<shared_ptr<class Vehicle>> vehicles;
	unique_ptr<RigidWorld> rigid_world;
	int step = 0;
	mutex step_mtx;

public:
	Arena(float step_time);
	void Start();
	void Step(StepInputs inputs);
};