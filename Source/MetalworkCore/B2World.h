#pragma once

#include "RigidWorld.h"

class B2World : public RigidWorld
{
	unique_ptr<b2World> world;
	vector<char> data;
	unordered_map<void*, int> allocs;

public:
	B2World(float gravity);
	void Capture() override;
	void Restore() override;
	void Step() override;
	vector<shared_ptr<RigidBody>> LoadModel(Json::Value& model_jval) override;
	string SaveToJson() override;
	void LoadFromJson(string_view json) override;
	void DebugDraw(const IDebugDrawer& drawer) override;
	~B2World();
};