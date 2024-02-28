#pragma once

#include "Body.h"
#include "Joint.h"

namespace Json { class Value; }
inline Name root_name = "root";

class RigidObject
{
public:
	class MetalActor* actor = nullptr;
	class RigidWorld* world = nullptr;
	vector<shared_ptr<class Joint>> joints;
	vector<shared_ptr<class Body>> parts;
	Body* root = nullptr;
	Name name;

public:
	RigidObject(const RigidObject&) = delete;
	RigidObject(Name name, MetalActor* actor = nullptr) : actor(actor), name(name) {}

	METALWORKCORE_API void DrawShapes(class IDebugDrawer& drawer);
	METALWORKCORE_API virtual void LoadModel(Json::Value& jmodel);

	void AddPart(shared_ptr<Body> body);
	Body* FindPart(const equality_comparable_with<Name> auto& name);
	shared_ptr<Body> RemovePart(const equality_comparable_with<Name> auto& name);

	void AddJoint(shared_ptr<Joint> joint, optional<Name> name = nullopt);
	Joint* FindJoint(const equality_comparable_with<Name> auto& name);
	shared_ptr<Joint> RemoveJoint(const equality_comparable_with<Name> auto& name);

	METALWORKCORE_API virtual void SetPosition(Vec2 position);
	METALWORKCORE_API Vec2 GetPosition();
	float GetMass();
	METALWORKCORE_API shared_ptr<RigidObject> Release();

	METALWORKCORE_API virtual ~RigidObject();
};

Body* RigidObject::FindPart(const equality_comparable_with<Name> auto& name)
{
	auto part_it = ranges::find_if(parts, [&](shared_ptr<Body>& part){ return part->name == name; });
	return part_it != parts.end() ? part_it->get() : nullptr;
}

shared_ptr<Body> RigidObject::RemovePart(const equality_comparable_with<Name> auto& name)
{
	auto part_it = ranges::find_if(parts, [&](shared_ptr<Body>& p){ return p->name == name; });
	shared_ptr<Body> body = move(*part_it);
	parts.erase(part_it);
	body->object = nullptr;
	return body;
}

Joint* RigidObject::FindJoint(const equality_comparable_with<Name> auto& name)
{
	auto joint_it = ranges::find_if(joints, [&](shared_ptr<Joint>& joint){ return joint->name = name; });
	return joint_it != joints.end() ? joint_it->get() : nullptr;
}

shared_ptr<Joint> RigidObject::RemoveJoint(const equality_comparable_with<Name> auto& name)
{
	auto joint_it = ranges::find_if(joints, [&](shared_ptr<Joint>& j){ return j->name == name; });
	shared_ptr<Joint> joint = move(*joint_it);
	joints.erase(joint_it);
	return joint;
}
