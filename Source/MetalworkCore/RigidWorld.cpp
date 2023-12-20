#include "RigidWorld.h"

RigidObject* RigidWorld::AddObject(shared_ptr<RigidObject> obj)
{
	return objects.emplace_back(move(obj)).get();
}

shared_ptr<RigidObject> RigidWorld::RemoveObject(RigidObject* obj)
{
	auto obj_it = ranges::find_if(objects, [&](shared_ptr<RigidObject>& o){ return o.get() == obj; });
	shared_ptr<RigidObject> removed_obj = move(*obj_it);
	objects.erase(obj_it);
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
	vec2 shift = position - root->GetPosition() + root_shift;
	for (RigidBody* part : parts | cptr)
		part->SetPosition(part->GetPosition() + shift);
}

void RigidObject::LoadModel(RigidWorld* world, Json::Value& jmodel, string_view root_name)
{
	parts = world->LoadModel(jmodel);
	ranges::for_each(parts | cptr, [&](RigidBody* part){ part->object = this; });

	if (!root_name.empty())
	{
		root = FindPart(root_name);
		if (!root) throw data_error("Root name {} not found.", root_name);
		root_shift = root->GetPosition();
	}
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
