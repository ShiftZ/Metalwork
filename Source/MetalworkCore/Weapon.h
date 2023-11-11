#pragma once

class Weapon
{
public:
	unique_ptr<class RigidObject> body;

public:
	Weapon(class Arena* arena, string_view model_name);
};