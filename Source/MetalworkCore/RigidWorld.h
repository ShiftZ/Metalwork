#pragma once

#include "Name.h"

class Body;
class Joint;
namespace Json { class Value; }

class RigidWorld
{
	vector<shared_ptr<class RigidObject>> objects;

public:
	int step = 0;
	int captured_step = -1;

public:
	template<typename ObjectType>
	ObjectType* AddObject(shared_ptr<ObjectType> obj)
		{ return (ObjectType*)AddObject(static_pointer_cast<RigidObject>(move(obj))); }

	METALWORKCORE_API RigidObject* AddObject(shared_ptr<RigidObject> obj);
	METALWORKCORE_API shared_ptr<RigidObject> RemoveObject(RigidObject* obj);
	METALWORKCORE_API RigidObject* FindObject(Name name);
	METALWORKCORE_API auto GetObjects() { return objects | cptr; }

	virtual void Capture() = 0;
	virtual void Restore() = 0;
	virtual void Step() = 0;
	virtual shared_ptr<Joint> CreateRevoluteJoint(Body* bodyA, Body* bodyB, vec2 anchorA, optional<vec2> anchorB = nullopt) = 0;
	virtual shared_ptr<Joint> CreateDistantJoint(Body* bodyA, Body* bodyB, float min, float max) = 0;
	virtual pair<vector<shared_ptr<Body>>, vector<shared_ptr<Joint>>> LoadModel(Json::Value& model) = 0;
	virtual string SaveToJson() = 0;
	virtual void LoadFromJson(string_view json) = 0;
	virtual void DebugDraw(const class IDebugDrawer& drawer) = 0;

	virtual vec2 GetGravity() = 0;

	virtual ~RigidWorld() = default;
};

class Body
{
public:
	class RigidObject* object = nullptr;
	Name name, model;
	vec2 offset;

public:
	Body(Name name, Name model, vec2 shift) : name(name), model(model), offset(shift) {}

	virtual vec2 GetPosition() = 0;
	virtual void SetPosition(vec2 position) = 0;
	virtual float GetAngle() = 0;
	virtual float GetMass() = 0;
	virtual float GetInertia() = 0;
	virtual vec2 GetVelocity() = 0;
	virtual float GetAngVelocity() = 0;
	virtual void SetGravityScale(float scale) = 0;
	virtual void SetAngDamping(float factor) = 0;
	virtual void SetLinearDamping(float factor) = 0;
	virtual void ApplyForce(vec2 force) = 0;
	virtual void ApplyForce(vec2 force, vec2 point) = 0;
	virtual void ApplyTorque(float torque) = 0;
	virtual void DrawShapes(class IDebugDrawer& drawer) = 0;

	virtual ~Body() = default;
};

class RigidObject
{
public:
	class MetalActor* actor = nullptr;
	RigidWorld* world = nullptr;
	vector<shared_ptr<Joint>> joints;
	vector<shared_ptr<Body>> parts;
	Body* root = nullptr;
	Name name;

public:
	RigidObject(const RigidObject&) = delete;
	RigidObject(Name name, MetalActor* actor = nullptr) : actor(actor), name(name) {}

	METALWORKCORE_API void DrawShapes(IDebugDrawer& drawer);
	METALWORKCORE_API void LoadModel(Json::Value& jmodel);

	void AddPart(shared_ptr<Body> body);
	shared_ptr<Body> RemovePart(Body* part);
	Body* FindPart(Name part_name);
	METALWORKCORE_API void SetPosition(vec2 position);
	METALWORKCORE_API vec2 GetPosition();
	float GetMass();
	METALWORKCORE_API shared_ptr<RigidObject> Release();

	METALWORKCORE_API virtual ~RigidObject();
};

class Joint
{
public:
	virtual float GetForce() = 0;
	virtual ~Joint() = default;
};

class MetalActor
{
public:
	virtual void ReleaseRig() = 0;
};