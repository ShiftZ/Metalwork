#pragma once

namespace Json { class Value; }

class RigidWorld
{
public:
	int step = 0;
	int captured_step = -1;

public:
	virtual void Capture() = 0;
	virtual void Restore() = 0;
	virtual void Step() = 0;
	virtual unordered_map<string, shared_ptr<class RigidBody>> LoadModel(Json::Value& model) = 0;
	virtual string SaveToJson() = 0;
	virtual void LoadFromJson(string_view json) = 0;
	virtual ~RigidWorld() = default;
};

class RigidBody
{
public:
	class RigidObject* object = nullptr;
	string model_name;

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
	unordered_map<string, shared_ptr<RigidBody>> parts;
	RigidBody* root = nullptr;
	vec2 root_shift = nullvec;

public:
	METALWORKCORE_API void DrawShapes(IDebugDrawer& drawer);
	void SetPosition(vec2 position);
	void LoadModel(RigidWorld* world, Json::Value& model, string root_name = {});
	virtual ~RigidObject() = default;
};
