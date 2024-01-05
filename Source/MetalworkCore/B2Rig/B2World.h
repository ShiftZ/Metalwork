#pragma once

#include "RigidWorld.h"

class B2World : public RigidWorld
{
	unique_ptr<b2World> b2world;
	vector<char> data;
	unordered_map<void*, int> allocs;

public:
	B2World(float gravity);
	void Capture() override;
	void Restore() override;
	void Step() override;

	shared_ptr<Joint> CreateRevoluteJoint(Body* bodyA, Body* bodyB, vec2 anchorA, optional<vec2> anchorB) override;
	shared_ptr<Joint> CreateDistantJoint(Body* bodyA, Body* bodyB, float min, float max) override;

	pair<vector<shared_ptr<Body>>, vector<shared_ptr<Joint>>> LoadModel(Json::Value& model_jval) override;
	string SaveToJson() override;
	void LoadFromJson(string_view json) override;
	void DebugDraw(const IDebugDrawer& drawer) override;

	vec2 GetGravity() override;

	~B2World();
};