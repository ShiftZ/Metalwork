#pragma once

#include "Name.h"

namespace Json { class Value; }

class RigidWorld
{
public:
	int step = 0;
	int captured_step = -1;
	vector<shared_ptr<class RigidObject>> objects;

public:
	RigidObject* FindObject(string_view name);
	shared_ptr<RigidObject> RemoveObject(RigidObject* obj);

	virtual void Capture() = 0;
	virtual void Restore() = 0;
	virtual void Step() = 0;
	virtual vector<shared_ptr<class RigidBody>> LoadModel(Json::Value& model) = 0;
	virtual string SaveToJson() = 0;
	virtual void LoadFromJson(string_view json) = 0;
	virtual ~RigidWorld() = default;
};

class RigidBody
{
public:
	class RigidObject* object = nullptr;
	Name name, model;

public:
	virtual vec2 GetPosition() = 0;
	virtual void SetPosition(vec2 position) = 0;
	virtual float GetAngle() = 0;
	virtual void JoinRevolute(RigidBody* with_body, vec2 anchorA, optional<vec2> anchorB = nullopt) = 0;
	virtual void JoinDistant(RigidBody* with, vec2 anchor, float min, float max) = 0;
	virtual void ApplyForce(vec2 force) = 0;
	virtual void DrawShapes(class IDebugDrawer& drawer) = 0;

	virtual ~RigidBody() = default;
};

class RigidObject
{
public:
	class MetalActor* actor = nullptr;
	vector<shared_ptr<RigidBody>> parts;
	RigidBody* root = nullptr;
	vec2 root_shift = nullvec;
	Name name;

public:
	METALWORKCORE_API void DrawShapes(IDebugDrawer& drawer);
	METALWORKCORE_API void LoadModel(RigidWorld* world, string_view jmodel_str, string_view root_name = {});

	RigidBody* FindPart(string_view part_name);
	shared_ptr<RigidBody> RemovePart(RigidBody* part);
	void SetPosition(vec2 position);

	virtual ~RigidObject() = default;
};

class MetalActor
{
public:
	virtual string GetName() = 0;
};