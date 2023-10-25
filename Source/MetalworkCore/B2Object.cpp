#include "B2Object.h"

#include "B2World.h"
#include "b2dJson/b2dJson.h"

B2Body::B2Body(RigidObject* object, b2Body* b2body): RigidBody(object, b2body)
{
	b2body->GetUserData() = this;
}

vec2 B2Body::GetPosition()
{
	return xbody->GetPosition();
}

vec3 B2Body::GetPosition3D()
{
	return {xbody->GetPosition().x, 0.f, xbody->GetPosition().y};
}

float B2Body::GetAngle()
{
	return 180.f * xbody->GetAngle() / numbers::pi;
}

void B2Body::JoinRevolute(RigidBody* with, vec2 anchorA, optional<vec2> anchorB)
{
	b2RevoluteJointDef def;
	if (!anchorB)
		def.Initialize(xbody, with->xbody, anchorA);
	else
	{
		def.bodyA = xbody;
		def.bodyB = with->xbody;
		def.localAnchorA = anchorA;
		def.localAnchorB = *anchorB;
	}

	xbody->GetWorld()->CreateJoint(&def);
}

void B2Body::JoinDistant(RigidBody* with, vec2 anchor, float min, float max)
{
	b2DistanceJointDef def;
	def.Initialize(xbody, with->xbody, xbody->GetLocalPoint(anchor), with->xbody->GetLocalPoint(anchor));
	def.minLength = min;
	def.maxLength = max;
	def.stiffness = 2;
	def.damping = 0.5;
}

void B2Body::ApplyForce(vec2 force)
{
	xbody->ApplyForceToCenter(force, true);
}

B2Object::B2Object(B2World* world, Json::Value model, string_view root_name)
{
	b2dJson json;
	json.readFromValue(model, world->xworld.get());

	b2Body* b2root = nullptr;
	if (!root_name.empty())
	{
		b2root = json.getBodyByName(string(root_name));
		if (!b2root) throw logic_error(format("Root part '{}' not found", root_name));
		root_shift = b2root->GetPosition();
	}

	vector<b2Body*> bodies;
	json.getAllBodies(bodies);

	for (b2Body* body : bodies)
	{
		shared_ptr part = make_shared<B2Body>(this, body);
		part->model_name = json.getCustomString(body, "Model");
		if (body == b2root) root = part.get();
		parts.emplace(json.getBodyName(body), move(part));
	}
}

void B2Object::SetPosition(vec2 position)
{
	b2Vec2 shift = b2Vec2(position.x, position.y) - root->GetPosition() + root_shift;
	for (b2Body* part : Parts())
		part->SetTransform(part->GetPosition() + shift, 0);
}
