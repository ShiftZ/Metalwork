#include "B2Body.h"

#include "B2World.h"
#include "CoreInterface.h"

B2Body::B2Body(b2Body* body, Name name, Name model) : RigidBody(name, model), body(body)
{
	body->GetUserData() = this;
}

vec2 B2Body::GetPosition()
{
	return body->GetPosition();
}

void B2Body::SetPosition(vec2 position)
{
	body->SetTransform(position, body->GetAngle());
}

float B2Body::GetAngle()
{
	return 180.f * body->GetAngle() / numbers::pi;
}

void B2Body::JoinRevolute(RigidBody* with, vec2 anchorA, optional<vec2> anchorB)
{
	b2RevoluteJointDef def;
	if (!anchorB)
		def.Initialize(body, GetB2Body(with), anchorA);
	else
	{
		def.bodyA = body;
		def.bodyB = GetB2Body(with);
		def.localAnchorA = anchorA;
		def.localAnchorB = *anchorB;
	}

	body->GetWorld()->CreateJoint(&def);
}

void B2Body::JoinDistant(RigidBody* with, vec2 anchor, float min, float max)
{
	b2DistanceJointDef def;
	def.Initialize(body, GetB2Body(with), body->GetLocalPoint(anchor), GetB2Body(with)->GetLocalPoint(anchor));
	def.minLength = min;
	def.maxLength = max;
	def.stiffness = 2;
	def.damping = 0.5;
}

void B2Body::ApplyForce(vec2 force)
{
	body->ApplyForceToCenter(force, true);
}

void B2Body::DrawShapes(IDebugDrawer& drawer)
{
	const b2Transform& transform = body->GetTransform();

	Color color(0, 1, 1, 1);

	for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
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

		case b2Shape::e_edge:
		{
			b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
			drawer.Edge(edge->m_vertex1, edge->m_vertex2);
			break;
		}
		}
	}
}

B2Body::~B2Body()
{
	if (body) body->GetWorld()->DestroyBody(body);
}
