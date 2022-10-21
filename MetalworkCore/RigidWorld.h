#pragma once

#include <unordered_map>
#include <memory>
#include <optional>
#include "vector2.h"
#include "vector3.h"

using namespace std;

using XWorld = class b2World;
using XBody = class b2Body;

class RigidWorld
{
public:
	unique_ptr<XWorld> xworld;
	int step = 0;
	int captured_step = -1;

public:
	virtual void Capture() = 0;
	virtual void Restore() = 0;
	virtual void Step() = 0;
	virtual ~RigidWorld() = default;
};

class RigidBody
{
public:
	class RigidObject* object;
	XBody* xbody;
	string model_name;

public:
	RigidBody(RigidObject* object, XBody* xbody) : object(object), xbody(xbody) {}

	virtual vec2 GetPosition() = 0;
	virtual vec3 GetPosition3D() = 0;
	virtual void JoinRevolute(RigidBody* with_body, vec2 anchorA, optional<vec2> anchorB = nullopt) = 0;
	virtual void JoinDistant(RigidBody* with, vec2 anchor, float min, float max) = 0;
	virtual void ApplyForce(vec2 force) = 0;
	virtual ~RigidBody() = default;
};

class RigidObject
{
public:
	unordered_map<string, shared_ptr<RigidBody>> parts;
	RigidBody* root = nullptr;
	vec2 root_shift = nullvec;

public:
	virtual void SetPosition(vec2 pos) = 0;
	virtual ~RigidObject() = default;
};