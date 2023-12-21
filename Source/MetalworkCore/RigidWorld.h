#pragma once

#include "Name.h"

namespace Json { class Value; }

class RigidWorld
{
	vector<shared_ptr<class RigidObject>> objects;

public:
	int step = 0;
	int captured_step = -1;

public:
	METALWORKCORE_API RigidObject* AddObject(shared_ptr<RigidObject> obj);
	METALWORKCORE_API shared_ptr<RigidObject> RemoveObject(RigidObject* obj);
	METALWORKCORE_API RigidObject* FindObject(Name name);
	METALWORKCORE_API auto GetObjects() { return objects | cptr; }

	virtual void Capture() = 0;
	virtual void Restore() = 0;
	virtual void Step() = 0;
	virtual vector<shared_ptr<class RigidBody>> LoadModel(Json::Value& model) = 0;
	virtual string SaveToJson() = 0;
	virtual void LoadFromJson(string_view json) = 0;
	virtual void SetDebugDrawer(class IDebugDrawer* drawer) = 0;
	virtual void DebugDraw() = 0;
	virtual ~RigidWorld() = default;
};

class RigidBody
{
public:
	class RigidObject* object = nullptr;
	Name name, model;

public:
	RigidBody(Name name, Name model) : name(name), model(model) {}

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
	RigidWorld* world = nullptr;
	vector<shared_ptr<RigidBody>> parts;
	RigidBody* root = nullptr;
	vec2 root_shift = nullvec;
	Name name;

public:
	RigidObject() = default;
	RigidObject(Name name) : name(name) {}
	RigidObject(MetalActor* actor) : actor(actor) {}

	METALWORKCORE_API void DrawShapes(IDebugDrawer& drawer);
	METALWORKCORE_API void LoadModel(RigidWorld* world, Json::Value& jmodel, string_view root_name = {});

	void AddPart(shared_ptr<RigidBody> body);
	shared_ptr<RigidBody> RemovePart(RigidBody* part);
	RigidBody* FindPart(Name part_name);
	void SetPosition(vec2 position);

	METALWORKCORE_API ~RigidObject();
};

class MetalActor
{
public:
	RigidObject* Rig = nullptr;
};