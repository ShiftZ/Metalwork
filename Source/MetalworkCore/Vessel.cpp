#pragma warning(disable:4458)

#include "Vessel.h"
#include "Arena.h"

Vessel::Vessel(Arena* arena, string_view model_name)
{
	path model_path = "../../Content/Vessels/"s + model_name.data() + ".json";
	ifstream file(model_path);
	string json_string;
	json_string.resize(file_size(model_path));
	file.read(json_string.data(), json_string.size());

	Json::Value jval;
	Json::Reader reader;
	reader.parse(json_string, jval);

	arena->rigid_world->LoadModel(*this, jval, "vehicle");
}

void Vessel::AttachWeapon(shared_ptr<Weapon> weapon)
{
	root->JoinRevolute(weapon->root, {0, -2}, -weapon->root_shift);
	this->weapon = move(weapon);
}

void Vessel::SetPosition(vec2 position)
{
	RigidObject::SetPosition(position);
	if (weapon) weapon->SetPosition(position);
}

