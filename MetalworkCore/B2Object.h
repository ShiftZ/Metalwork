#pragma once

#include <ranges>

#include "b2dJson/b2dJson.h"

#include "RigidWorld.h"

class B2Object;

class B2Body : public RigidBody
{
public:
	B2Body(RigidObject* object, b2Body* body);
	vec2 GetPosition() override;
	vec3 GetPosition3D() override;
	void JoinRevolute(RigidBody* with, vec2 anchorA, optional<vec2> anchorB = nullopt) override;
	void JoinDistant(RigidBody* with, vec2 anchor, float min, float max) override;
};

class B2Object : public RigidObject
{
public:
	B2Object(class B2World* world, Json::Value model, string_view root_name = {});

	void SetPosition(vec2 position) override;
	auto Parts() { return views::values(parts) | views::transform([](shared_ptr<RigidBody>& b){ return b->xbody; }); }
};


