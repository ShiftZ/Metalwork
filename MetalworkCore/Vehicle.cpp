#include "Vehicle.h"

#include <fstream>
#include <filesystem>
#include <b2dJson/b2dJson.h>

#include "Arena.h"
#include "B2Object.h"
#include "B2World.h"

using namespace std::filesystem;

Vehicle::Vehicle(Arena* arena, string_view model_name)
{
	path model_path = "../../Content/Vehicles/"s + model_name.data() + ".json";
	ifstream file(model_path);
	string json_string;
	json_string.resize(file_size(model_path));
	file.read(json_string.data(), json_string.size());

	Json::Value jval;
	Json::Reader reader;
	reader.parse(json_string, jval);

	body = make_unique<B2Object>((B2World*)arena->rigid_world.get(), jval, "vehicle");
}

void Vehicle::AttachWeapon(shared_ptr<Weapon> weapon)
{
	body->root->JoinRevolute(weapon->body->root, nullvec, -weapon->body->root_shift);
	this->weapon = move(weapon);
}

void Vehicle::SetPosition(vec2 position)
{
	body->SetPosition(position);
	if (weapon) weapon->body->SetPosition(position);
}

