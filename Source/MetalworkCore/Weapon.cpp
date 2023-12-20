#include "Weapon.h"

#include "Arena.h"
#include "CoreInterface.h"

Weapon::Weapon(Arena* arena, Name model_name)
{
	LoadModel(arena->rigid_world.get(), GetJson(model_name), "root");
}
