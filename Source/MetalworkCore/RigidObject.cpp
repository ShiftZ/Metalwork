#include "Body.h"
#include "Joint.h"
#include "RigidObject.h"
#include "RigidWorld.h"

void RigidObject::DrawShapes(IDebugDrawer& drawer)
{
	for (Body* body : parts | cptr)
		body->DrawShapes(drawer);

	if (root) root->DrawShapes(drawer);
}

void RigidObject::SetPosition(Vec2 position)
{
	if (root)
	{
		Vec2 shift = position - root->GetPosition() + root->offset;
		for (Body* part : parts | cptr)
			part->SetPosition(part->GetPosition() + shift);
	}
	else
	{
		for (Body* part : parts | cptr)
			part->SetPosition(position + part->offset);
	}
}

Vec2 RigidObject::GetPosition()
{
	Body* body = root ? root : parts.front().get();
	return body->GetPosition() - body->offset;
}

float RigidObject::GetMass()
{
	return ranges::fold_left(parts | cptr, 0.f,
		[](float acc, Body* body){ return acc + body->GetMass(); });
}

shared_ptr<RigidObject> RigidObject::Release()
{
	return world->RemoveObject(this);
}

RigidObject::~RigidObject()
{
	if (actor) actor->ReleaseRig();
}

void RigidObject::LoadModel(Json::Value& jmodel)
{
	joints.clear();
	parts.clear();

	tie(parts, joints) = world->LoadModel(jmodel);
	for (Body* p : parts | cptr) p->object = this;

	root = FindPart(root_name);
	if (!root) root = parts.front().get();
}

void RigidObject::AddPart(shared_ptr<Body> body)
{
	body->object = this;
	parts.push_back(move(body));
}

void RigidObject::AddJoint(shared_ptr<Joint> joint, optional<Name> name)
{
	if (name) joint->name = *name;
	joints.push_back(move(joint));
}