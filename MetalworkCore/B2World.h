#pragma once

#include <unordered_map>

#include "RigidWorld.h"

using namespace std;

class B2World : public RigidWorld
{
	vector<char> data;
	unordered_map<void*, int> allocs;
	float step_time;

public:
	B2World(float step_time);
	void Capture() override;
	void Restore() override;
	void Step();
};

