#include "B2Body.h"

#include "CoreInterface.h"

B2Body::B2Body(b2Body* b2body, Name name) : Body(name, b2body->GetPosition()), b2body(b2body)
{
	b2body->GetUserData() = this;
}

B2Body::~B2Body()
{
	if (b2body) b2body->GetWorld()->DestroyBody(b2body);
}

Vec2 Body::GetPosition()
{
	return GetB2Body(this)->GetPosition();
}

void Body::SetPosition(Vec2 position)
{
	GetB2Body(this)->SetTransform(position, GetB2Body(this)->GetAngle());
}

Float Body::GetAngle()
{
	return GetB2Body(this)->GetAngle();
}

Float Body::GetAngleMod()
{
	return remainderf(GetB2Body(this)->GetAngle(), 2*pi);
}

Float Body::GetMass()
{
	return GetB2Body(this)->GetMass();
}

Float Body::GetInertia()
{
	return GetB2Body(this)->GetInertia();
}

Vec2 Body::GetVelocity()
{
	return GetB2Body(this)->GetLinearVelocity();
}

Float Body::GetAngVelocity()
{
	return GetB2Body(this)->GetAngularVelocity();
}

void Body::SetGravityScale(Float scale)
{
	GetB2Body(this)->SetGravityScale(scale);
}

void Body::SetAngDamping(Float factor)
{
	GetB2Body(this)->SetAngularDamping(factor);
}

void Body::SetLinearDamping(Float factor)
{
	GetB2Body(this)->SetLinearDamping(factor);
}

void Body::ApplyForce(Vec2 force)
{
	GetB2Body(this)->ApplyForceToCenter(force, false);
}

void Body::ApplyForce(Vec2 force, Vec2 point)
{
	GetB2Body(this)->ApplyForce(force, point, false);
}

void Body::ApplyImpulse(Vec2 impulse, Vec2 point)
{
	GetB2Body(this)->ApplyLinearImpulse(impulse, point, true);
}

void Body::ApplyTorque(Float torque)
{
	GetB2Body(this)->ApplyTorque(torque, false);
}

void Body::SetMovablity(Movability movability)
{
	GetB2Body(this)->SetType(b2BodyType(movability));
}

Movability Body::GetMovability()
{
	return (Movability)GetB2Body(this)->GetType();
}

void Body::DrawShapes(IDebugDrawer& drawer)
{
	const b2Transform& transform = GetB2Body(this)->GetTransform();

	Color color(0, 1, 1, 1);

	for (b2Fixture* fixture = GetB2Body(this)->GetFixtureList(); fixture; fixture = fixture->GetNext())
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
