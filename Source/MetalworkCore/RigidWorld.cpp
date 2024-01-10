#include "RigidWorld.h"

RigidObject* RigidWorld::AddObject(shared_ptr<RigidObject> obj)
{
	obj->world = this;
	return objects.emplace_back(move(obj)).get();
}

shared_ptr<RigidObject> RigidWorld::RemoveObject(RigidObject* obj)
{
	auto obj_it = ranges::find_if(objects, [&](shared_ptr<RigidObject>& o){ return o.get() == obj; });
	shared_ptr<RigidObject> removed_obj = move(*obj_it);
	objects.erase(obj_it);
	removed_obj->world = nullptr;
	return removed_obj;
}

RigidObject* RigidWorld::FindObject(Name name)
{
	auto obj_it = ranges::find_if(objects, [&](shared_ptr<RigidObject>& obj){ return obj->name == name; });
	return obj_it != objects.end() ? obj_it->get() : nullptr;
}

void RigidObject::DrawShapes(IDebugDrawer& drawer)
{
	for (Body* body : parts | cptr)
		body->DrawShapes(drawer);

	if (root) root->DrawShapes(drawer);
}

void RigidObject::SetPosition(vec2 position)
{
	if (root)
	{
		vec2 shift = position - root->GetPosition() + root->offset;
		for (Body* part : parts | cptr)
			part->SetPosition(part->GetPosition() + shift);
	}
	else
	{
		for (Body* part : parts | cptr)
			part->SetPosition(position + part->offset);
	}
}

vec2 RigidObject::GetPosition()
{
	Body* body = root ? root : parts.front().get();
	return body->GetPosition() - body->offset;
}

float RigidObject::GetMass()
{
	return ranges::fold_left(parts | cptr, 0.f,
		[](float acc, Body* body){ return acc + body->GetMass(); });
}

shared_ptr<RigidObject> RigidObject::Release()
{
	return world->RemoveObject(this);
}

RigidObject::~RigidObject()
{
	if (actor) actor->ReleaseRig();
}

void RigidObject::LoadModel(Json::Value& jmodel)
{
	joints.clear();
	parts.clear();

	tie(parts, joints) = world->LoadModel(jmodel);
	for (Body* p : parts | cptr) p->object = this;

	root = FindPart(root_name);
	if (!root) root = parts.front().get();
}

void RigidObject::AddPart(shared_ptr<Body> body)
{
	body->object = this;
	parts.push_back(move(body));
}

Body* RigidObject::FindPart(Name part_name)
{
	auto part_it = ranges::find_if(parts, [&](shared_ptr<Body>& part){ return part->name == part_name; });
	return part_it != parts.end() ? part_it->get() : nullptr;
}

shared_ptr<Body> RigidObject::RemovePart(Name part_name)
{
	auto part_it = ranges::find_if(parts, [&](shared_ptr<Body>& p){ return p->name == part_name; });
	shared_ptr<Body> body = move(*part_it);
	parts.erase(part_it);
	body->object = nullptr;
	return body;
}

void RigidObject::AddJoint(shared_ptr<Joint> joint, optional<Name> name)
{
	if (name) joint->name = *name;
	joints.push_back(move(joint));
}

Joint* RigidObject::FindJoint(Name joint_name)
{
	auto joint_it = ranges::find_if(joints, [&](shared_ptr<Joint>& joint){ return joint->name = joint_name; });
	return joint_it != joints.end() ? joint_it->get() : nullptr;
}

shared_ptr<Joint> RigidObject::RemoveJoint(Name joint_name)
{
	auto joint_it = ranges::find_if(joints, [&](shared_ptr<Joint>& j){ return j->name == joint_name; });
	shared_ptr<Joint> joint = move(*joint_it);
	joints.erase(joint_it);
	return joint;
}
