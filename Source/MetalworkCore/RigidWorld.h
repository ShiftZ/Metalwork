#pragma once
#pragma warning(disable:4458)

#include "Name.h"

class Body;
class Joint;
namespace Json { class Value; }

inline Name root_name = "root";

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
	virtual shared_ptr<Joint> CreateRevoluteJoint(Body* bodyA, Body* bodyB, Vec2 anchorA, optional<Vec2> anchorB = nullopt) = 0;
	virtual shared_ptr<Joint> CreateDistantJoint(Body* bodyA, Body* bodyB, Float length, 
												 optional<Vec2> anchorA = nullopt, optional<Vec2> anchorB = nullopt) = 0;
	virtual pair<vector<shared_ptr<Body>>, vector<shared_ptr<Joint>>> LoadModel(Json::Value& model) = 0;
	virtual string SaveToJson() = 0;
	virtual void LoadFromJson(string_view json) = 0;
	virtual void DebugDraw(const class IDebugDrawer& drawer) = 0;

	virtual Vec2 GetGravity() = 0;

	virtual ~RigidWorld() = default;
};

class Body
{
public:
	class RigidObject* object = nullptr;
	Name name, model;
	Vec2 offset;

public:
	Body(Name name, Name model, Vec2 shift) : name(name), model(model), offset(shift) {}

	virtual Vec2 GetPosition() = 0;
	virtual void SetPosition(Vec2 position) = 0;
	virtual Float GetAngle() = 0;
	virtual Float GetAngleMod() = 0;
	virtual Float GetMass() = 0;
	virtual Float GetInertia() = 0;
	virtual Vec2 GetVelocity() = 0;
	virtual Float GetAngVelocity() = 0;
	virtual void SetGravityScale(Float scale) = 0;
	virtual void SetAngDamping(Float factor) = 0;
	virtual void SetLinearDamping(Float factor) = 0;
	virtual void ApplyForce(Vec2 force) = 0;
	virtual void ApplyForce(Vec2 force, Vec2 point) = 0;
	virtual void ApplyTorque(Float torque) = 0;
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
	METALWORKCORE_API virtual void LoadModel(Json::Value& jmodel);

	void AddPart(shared_ptr<Body> body);
	Body* FindPart(Name part_name);
	shared_ptr<Body> RemovePart(Name part_name);
	
	void AddJoint(shared_ptr<Joint> joint, optional<Name> name = nullopt);
	Joint* FindJoint(Name joint_name);
	shared_ptr<Joint> RemoveJoint(Name joint_name);

	METALWORKCORE_API virtual void SetPosition(Vec2 position);
	METALWORKCORE_API Vec2 GetPosition();
	float GetMass();
	METALWORKCORE_API shared_ptr<RigidObject> Release();

	METALWORKCORE_API virtual ~RigidObject();
};

class Joint
{
public:
	Name name;

public:
	virtual Body* GetBodyA() = 0;
	virtual Body* GetBodyB() = 0;
	virtual Float GetForce() = 0;
	virtual ~Joint() = default;
};

class MetalActor
{
public:
	virtual void ReleaseRig() = 0;
};