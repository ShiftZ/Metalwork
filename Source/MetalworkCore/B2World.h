#pragma once

#include "RigidWorld.h"

using namespace std;

class B2World : public RigidWorld
{
	vector<char> data;
	unordered_map<void*, int> allocs;
	float step_time;

public:
	B2World(float step_time, float gravity);
	void Capture() override;
	void Restore() override;
	void Step();
};

