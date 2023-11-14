#include "RigidWorld.h"

void RigidObject::DrawShapes(IDebugDrawer& drawer)
{
	for (RigidBody* body : parts | views::values | cptr)
		body->DrawShapes(drawer);

	if (root) root->DrawShapes(drawer);
}

void RigidObject::SetPosition(vec2 position)
{
	vec2 shift = position - root->GetPosition() + root_shift;
	for (RigidBody* part : parts | views::values | cptr)
		part->SetPosition(part->GetPosition() + shift);
}

void RigidObject::LoadModel(RigidWorld* world, Json::Value& model, string root_name)
{
	parts = world->LoadModel(model);

	for (RigidBody* part : parts | views::values | cptr)
		part->object = this;

	if (!root_name.empty())
	{
		auto root_it = parts.find(root_name);
		if (root_it == parts.end())
			throw data_error("Root name {} not found.", root_name);
		root = root_it->second.get();
		root_shift = root->GetPosition();
	}
}

shared_ptr<RigidBody> RigidObject::RemovePart(RigidBody* part)
{
	auto part_it = ranges::find_if(parts, [&](value<shared_ptr<RigidBody>> p){ return p->get() == part; });
	auto [_, body] = move(*part_it);
	parts.erase(part_it);
	body->object = nullptr;
	return body;
}
