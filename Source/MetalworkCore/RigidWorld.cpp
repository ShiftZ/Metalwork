#include "RigidWorld.h"

void RigidObject::DrawShapes(IDebugDrawer& drawer)
{
	for (RigidBody* body : parts | views::values | cptr)
		body->DrawShapes(drawer);

	if (root) root->DrawShapes(drawer);
}
