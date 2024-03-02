#pragma warning(disable:4458)

#include "Body.h"

void Body::SetRole(Role role)
{
	this->role = role;
	if (role == Prop) SetGravityScale(0);
}
