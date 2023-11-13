#pragma once

#include "RigidWorld.h"

class Weapon : public RigidObject
{
public:
	Weapon(class Arena* arena, string_view model_name);
};