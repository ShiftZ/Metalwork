#pragma once

#include "RigidWorld.h"

class B2Joint : public Joint
{
public:
	class b2Joint* b2joint;
	Name name;

public:
	B2Joint(b2Joint* b2joint, Name name = {});
	Body* GetBodyA() override;
	Body* GetBodyB() override;
	Float GetForce() override;
	~B2Joint();
};