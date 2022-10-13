#pragma once

#include <memory>
#include "RigidWorld.h"

using namespace std;

class Weapon
{
public:
	unique_ptr<RigidObject> body;

public:
	Weapon(class Arena* arena, string_view model_name);
};