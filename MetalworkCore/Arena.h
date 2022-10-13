#pragma once

#include <memory>
#include <vector>

#include "RigidWorld.h"

class Arena
{
public:
	struct PlayerInput
	{
		vec2i move = nullvec;
	};

public:
	vector<shared_ptr<class Vehicle>> vehicles;
	unique_ptr<RigidWorld> rigid_world;
	int step = 0;

public:
	Arena(float step_time);
	void CleanStep(vector<vector<PlayerInput>> inputs);
	void DirtyStep(vector<vector<PlayerInput>> inputs);
};