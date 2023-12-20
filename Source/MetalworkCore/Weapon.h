#pragma once

#include "RigidWorld.h"

class Weapon : public RigidObject
{
public:
	Weapon(class Arena* arena, Name model_name);
};