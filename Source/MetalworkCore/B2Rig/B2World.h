#pragma once

#include "RigidWorld.h"

class B2World : public RigidWorld, public b2ContactListener
{
	unique_ptr<b2World> b2world;
	vector<char> data;
	unordered_map<void*, int> allocs;

public:
	B2World(Float gravity);
	~B2World();

	void Capture() override;
	void Restore() override;
	void Step() override;

	shared_ptr<Joint> CreateRevoluteJoint(Body* bodyA, Body* bodyB, Vec2 anchorA, optional<Vec2> anchorB) override;
	shared_ptr<Joint> CreateDistantJoint(Body* bodyA, Body* bodyB, Float length, 
										 optional<Vec2> anchorA = nullopt, optional<Vec2> anchorB = nullopt) override;

	pair<vector<shared_ptr<Body>>, vector<shared_ptr<Joint>>> LoadModel(Json::Value& model_jval) override;
	string SaveToJson() override;
	void LoadFromJson(string_view json) override;
	void DebugDraw(const IDebugDrawer& drawer) override;

	Vec2 GetGravity() override;

private:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
	void PreSolve(b2Contact* contact, const b2Manifold* old_manifold) override;
	void PostSolve(Contact* contact, const b2ContactImpulse* impulse) override;
};