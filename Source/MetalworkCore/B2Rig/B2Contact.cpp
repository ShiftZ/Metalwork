#include "B2Contact.h"
#include "B2Body.h"

pair<Body*, Body*> GetBodies(Contact* contact)
{
	return {contact->GetFixtureA()->GetBody()->GetUserData(), contact->GetFixtureB()->GetBody()->GetUserData()};
}

int GetNumPoints(Contact* contact)
{
	return contact->GetManifold()->pointCount;
}

float GetNormalImpulseMag(Contact* contact, const void* data)
{
	b2ContactImpulse* contact_impulse = (b2ContactImpulse*)data;
	return ranges::fold_left(contact_impulse->normalImpulses, contact_impulse->normalImpulses + GetNumPoints(contact), 0.f, plus<float>());
}

vector<pair<Vec2, Vec2>> GetImpulses(Contact* contact, const void* data, Body* body)
{
	vector<pair<Vec2, Vec2>> impulses(contact->GetManifold()->pointCount);

	b2ContactImpulse* contact_impulse = (b2ContactImpulse*)data;
	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);

	Vec2 normal = (body == contact->GetFixtureA()->GetBody()->GetUserData()) ? -world_manifold.normal : world_manifold.normal;

	for (int i = 0; i < impulses.size(); i++)
		impulses[i] = {normal * contact_impulse->normalImpulses[i], world_manifold.points[i]};

	return impulses;
}

Vec2 GetLocation(Contact* contact)
{
	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);
	if (contact->GetManifold()->pointCount > 1)
	{
		for (int i = 1; i < contact->GetManifold()->pointCount; i++)
			world_manifold.points[0] += world_manifold.points[i];
		world_manifold.points[0] /= contact->GetManifold()->pointCount;
	}


	return world_manifold.points[0];
}