#include "B2Body.h"

#include "B2World.h"
#include "CoreInterface.h"

B2Body::B2Body(b2Body* b2body, Name name, Name model) : RigidBody(name, model, b2body->GetPosition()), b2body(b2body)
{
	b2body->GetUserData() = this;
}

vec2 B2Body::GetPosition()
{
	return b2body->GetPosition();
}

void B2Body::SetPosition(vec2 position)
{
	b2body->SetTransform(position, b2body->GetAngle());
}

float B2Body::GetAngle()
{
	return b2body->GetAngle();
}

float B2Body::GetMass()
{
	return b2body->GetMass();
}

float B2Body::GetInertia()
{
	return b2body->GetInertia();
}

vec2 B2Body::GetVelocity()
{
	return b2body->GetLinearVelocity();
}

float B2Body::GetAngVelocity()
{
	return b2body->GetAngularVelocity();
}

void B2Body::SetGravityScale(float scale)
{
	b2body->SetGravityScale(scale);
}

void B2Body::SetAngDamping(float factor)
{
	b2body->SetAngularDamping(factor);
}

void B2Body::SetLinearDamping(float factor)
{
	b2body->SetLinearDamping(factor);
}

void B2Body::JoinRevolute(RigidBody* with, vec2 anchorA, optional<vec2> anchorB)
{
	b2RevoluteJointDef def;
	if (!anchorB)
		def.Initialize(b2body, GetB2Body(with), anchorA);
	else
	{
		def.bodyA = b2body;
		def.bodyB = GetB2Body(with);
		def.localAnchorA = anchorA;
		def.localAnchorB = *anchorB;
	}

	b2body->GetWorld()->CreateJoint(&def);
}

void B2Body::JoinDistant(RigidBody* with, float min, float max)
{
	b2DistanceJointDef def;
	def.Initialize(b2body, GetB2Body(with), b2body->GetPosition(), GetB2Body(with)->GetPosition());
	def.minLength = min;
	def.maxLength = max;
	//def.stiffness = 0;
	//def.damping = 0;

	b2body->GetWorld()->CreateJoint(&def);
}

void B2Body::ApplyForce(vec2 force)
{
	b2body->ApplyForceToCenter(force, false);
}

void B2Body::ApplyForce(vec2 force, vec2 point)
{
	b2body->ApplyForce(force, point, false);
}

void B2Body::ApplyTorque(float torque)
{
	b2body->ApplyTorque(torque, false);
}

void B2Body::DrawShapes(IDebugDrawer& drawer)
{
	const b2Transform& transform = b2body->GetTransform();

	Color color(0, 1, 1, 1);

	for (b2Fixture* fixture = b2body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
		{
			b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();
			b2Vec2 center = transform.p + b2Mul(transform.q, circle->m_p);
			drawer.Circle(center, circle->m_radius, color);
			break;
		}

		case b2Shape::e_polygon:
		{	
			b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
			vector<vec2> verts(poly->m_count);
			for (int i = 0; i < poly->m_count; ++i)
				verts[i] = transform.p + b2Mul(transform.q, poly->m_vertices[i]);
			verts.push_back(verts[0]);
			drawer.Poly(verts, color);
			break;
		}
		}
	}
}

B2Body::~B2Body()
{
	if (b2body) b2body->GetWorld()->DestroyBody(b2body);
}
