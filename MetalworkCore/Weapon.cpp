#include <filesystem>
#include <fstream>

#include "b2dJson/b2dJson.h"
#include "B2Object.h"
#include "Arena.h"

#include "Weapon.h"

using namespace std::filesystem;

Weapon::Weapon(Arena* arena, string_view model_name)
{
	path model_path = "../../Content/Weapons/"s + model_name.data() + ".json";
	ifstream file(model_path);
	string json_string;
	json_string.resize(file_size(model_path));
	file.read(json_string.data(), json_string.size());

	Json::Value jval;
	Json::Reader reader;
	reader.parse(json_string, jval);

	body = make_unique<B2Object>((class B2World*)arena->rigid_world.get(), jval, "root");
}
