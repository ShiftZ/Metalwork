#include "Weapon.h"

#include "Arena.h"

Weapon::Weapon(Arena* arena, string_view model_name)
{
	path model_path = "../../Content/Weapons/"s + model_name.data() + ".json";
	ifstream file(model_path);
	string json_string;
	json_string.resize(file_size(model_path));
	file.read(json_string.data(), json_string.size());

	LoadModel(arena->rigid_world.get(), json_string, "root");
}
