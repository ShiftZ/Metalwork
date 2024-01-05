#include "B2Joint.h"
#include "CoreDefinitions.h"

B2Joint::B2Joint(b2Joint* b2joint, Name name) : b2joint(b2joint), name(name)
{
	b2joint->GetUserData() = this;
}

float B2Joint::GetForce()
{
	return b2joint->GetReactionForce(fps).Length();
}

B2Joint::~B2Joint()
{
	b2joint->GetBodyA()->GetWorld()->DestroyJoint(b2joint);
}
