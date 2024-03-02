#pragma once
#pragma warning(disable:4458)

#include "Contact.h"
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

	function<bool(Contact*)> begin_contact;
	function<void(Contact*)> end_contact;
	function<bool(Contact*, const void*)> pre_solve;
	function<void(Contact*, const void*)> post_solve;

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

class MetalActor
{
public:
	virtual void ReleaseRig() = 0;
};