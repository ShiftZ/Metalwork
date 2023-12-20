#pragma once

#include "RigidWorld.h"

class B2World : public RigidWorld
{
	unique_ptr<b2World> world;
	vector<char> data;
	unordered_map<void*, int> allocs;
	unique_ptr<class B2Draw> drawer;

public:
	B2World(float gravity);
	void Capture() override;
	void Restore() override;
	void Step() override;
	vector<shared_ptr<RigidBody>> LoadModel(Json::Value& model_jval) override;
	string SaveToJson() override;
	void LoadFromJson(string_view json) override;
	void SetDebugDrawer(IDebugDrawer* engine_drawer) override;
	void DebugDraw() override;
	~B2World();
};