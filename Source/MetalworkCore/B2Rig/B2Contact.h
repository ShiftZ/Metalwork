#pragma once

#include <box2d/b2_contact.h>
#include <box2d/b2_user_settings.h>

#include "B2Body.h"

inline Body* GetBodyA(Contact* contact)
{
	return contact->GetFixtureA()->GetBody()->GetUserData();
}

inline Body* GetBodyB(Contact* contact)
{
	return contact->GetFixtureB()->GetBody()->GetUserData();
}