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
	for (RigidBody* body : parts | cptr)
		body->DrawShapes(drawer);

	if (root) root->DrawShapes(drawer);
}

void RigidObject::SetPosition(vec2 position)
{
	if (root)
	{
		vec2 shift = position - root->GetPosition() + root->offset;
		for (RigidBody* part : parts | cptr)
			part->SetPosition(part->GetPosition() + shift);
	}
	else
	{
		for (RigidBody* part : parts | cptr)
			part->SetPosition(position + part->offset);
	}
}

RigidObject::~RigidObject()
{
	if (actor) actor->ReleaseRig();
}

void RigidObject::LoadModel(Json::Value& jmodel)
{
	parts.clear();

	parts = world->LoadModel(jmodel);
	for (RigidBody* p : parts | cptr) p->object = this;

	static const Name default_root_name = "root";
	root = FindPart(default_root_name);
	if (!root) root = parts.front().get();
}

void RigidObject::AddPart(shared_ptr<RigidBody> body)
{
	body->object = this;
	parts.push_back(move(body));
}

RigidBody* RigidObject::FindPart(Name part_name)
{
	auto part_it = ranges::find_if(parts, [&](shared_ptr<RigidBody>& part){ return part->name == part_name; });
	return part_it != parts.end() ? part_it->get() : nullptr;
}

shared_ptr<RigidBody> RigidObject::RemovePart(RigidBody* part)
{
	auto part_it = ranges::find_if(parts, [&](shared_ptr<RigidBody>& p){ return p.get() == part; });
	shared_ptr<RigidBody> body = move(*part_it);
	parts.erase(part_it);
	body->object = nullptr;
	return body;
}
