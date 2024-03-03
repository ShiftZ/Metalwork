#pragma once

#include "Body.h"

class B2Body final : public Body
{
public:
	b2Body* b2body;

public:
	B2Body(b2Body* b2body, Name name);
	~B2Body();
};

inline b2Body* GetB2Body(Body* body) { return static_cast<B2Body*>(body)->b2body; }
