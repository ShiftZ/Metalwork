#pragma warning(disable:4458)

#include "Vessel.h"

#include "Arena.h"
#include "B2Object.h"
#include "B2World.h"

using namespace std::filesystem;

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

	body = make_unique<B2Object>((B2World*)arena->rigid_world.get(), jval, "vehicle");
}

void Vessel::AttachWeapon(shared_ptr<Weapon> weapon)
{
	body->root->JoinRevolute(weapon->body->root, {0, -2}, -weapon->body->root_shift);
	this->weapon = move(weapon);
}

void Vessel::SetPosition(vec2 position)
{
	body->SetPosition(position);
	if (weapon) weapon->body->SetPosition(position);
}

