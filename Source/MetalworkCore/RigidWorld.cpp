#include "RigidWorld.h"
#include "RigidObject.h"

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