#include "B2Body.h"

#include "B2World.h"
#include "CoreInterface.h"

B2Body::B2Body(b2Body* b2body, Name name, Name model) : Body(name, model, b2body->GetPosition()), b2body(b2body)
{
	b2body->GetUserData() = this;
}

Vec2 B2Body::GetPosition()
{
	return b2body->GetPosition();
}

void B2Body::SetPosition(Vec2 position)
{
	b2body->SetTransform(position, b2body->GetAngle());
}

Float B2Body::GetAngle()
{
	return b2body->GetAngle();
}

Float B2Body::GetAngleMod()
{
	return remainderf(b2body->GetAngle(), 2*pi);
}

Float B2Body::GetMass()
{
	return b2body->GetMass();
}

Float B2Body::GetInertia()
{
	return b2body->GetInertia();
}

Vec2 B2Body::GetVelocity()
{
	return b2body->GetLinearVelocity();
}

Float B2Body::GetAngVelocity()
{
	return b2body->GetAngularVelocity();
}

void B2Body::SetGravityScale(Float scale)
{
	b2body->SetGravityScale(scale);
}

void B2Body::SetAngDamping(Float factor)
{
	b2body->SetAngularDamping(factor);
}

void B2Body::SetLinearDamping(Float factor)
{
	b2body->SetLinearDamping(factor);
}

void B2Body::ApplyForce(Vec2 force)
{
	b2body->ApplyForceToCenter(force, false);
}

void B2Body::ApplyForce(Vec2 force, Vec2 point)
{
	b2body->ApplyForce(force, point, false);
}

void B2Body::ApplyTorque(Float torque)
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
			vector<Vec2> verts(poly->m_count);
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
