#include "B2World.h"
#include "B2Body.h"
#include "B2Joint.h"
#include "B2Draw.h"
#include "RigidObject.h"
#include "CoreDefinitions.h"
#include "b2dJson/b2dJson.h"

B2World::B2World(Float gravity)
{
	b2world = make_unique<b2World>(b2Vec2(0, gravity));
	b2world->SetContactListener(this);
}

void B2World::Capture()
{
	for (void* ptr : b2releases)
	{
		allocs.erase(ptr);
		operator delete(ptr);
	}

	b2releases.clear();

	b2allocs.insert(allocs.begin(), allocs.end());

	int b2allocs_size = 0;
	for (int size : views::values(b2allocs))
		b2allocs_size += size;

	data.resize(b2allocs_size + sizeof(b2World));
	char* ptr = &data.front();
	
	memcpy(ptr, b2world.get(), sizeof(b2World));
	ptr += sizeof(b2World);

	for (auto& [alloc_ptr, size] : b2allocs)
	{
		memcpy(ptr, alloc_ptr, size);
		ptr += size;
	}

	allocs = move(b2allocs);
	captured_step = step;
}

void B2World::Restore()
{
	if (step == captured_step) return;

	for (void* ptr : views::keys(b2allocs))
		operator delete(ptr);

	b2allocs.clear();
	b2releases.clear();

	char* data_ptr = &data.front();
	memcpy(b2world.get(), data_ptr, sizeof(b2World));
	data_ptr += sizeof(b2World);

	for (auto& [ptr, size] : allocs)
	{
		memcpy(ptr, data_ptr, size);
		data_ptr += size;
	}
	
	step = captured_step;
}

void B2World::Step()
{
	b2world->Step((1000 / fps) * 0.001f, 8, 3);
	++step;
}

shared_ptr<Joint> B2World::CreateRevoluteJoint(Body* bodyA, Body* bodyB, Vec2 anchorA, optional<Vec2> anchorB)
{
	b2RevoluteJointDef def;
	if (!anchorB)
		def.Initialize(GetB2Body(bodyA), GetB2Body(bodyB), anchorA);
	else
	{
		def.bodyA = GetB2Body(bodyA);
		def.bodyB = GetB2Body(bodyB);
		def.localAnchorA = anchorA;
		def.localAnchorB = *anchorB;
	}

	b2Joint* b2joint = b2world->CreateJoint(&def);
	return make_shared<B2Joint>(b2joint);
}

shared_ptr<Joint> B2World::CreateDistantJoint(Body* bodyA, Body* bodyB, float length, optional<Vec2> anchorA, optional<Vec2> anchorB)
{
	b2DistanceJointDef def;
	Vec2 ancA = anchorA ? *anchorA : bodyA->GetPosition();
	Vec2 ancB = anchorB ? *anchorB : bodyB->GetPosition();

	def.Initialize(GetB2Body(bodyA), GetB2Body(bodyB), ancA, ancB);
	def.minLength = 0;
	def.maxLength = length;

	b2Joint* b2joint = b2world->CreateJoint(&def);
	return make_shared<B2Joint>(b2joint);
}

pair<vector<shared_ptr<Body>>, vector<shared_ptr<Joint>>> B2World::LoadModel(Json::Value& model_jval)
{
	vector<shared_ptr<Body>> bodies;
	vector<shared_ptr<Joint>> joints;

	b2dJson json;
	json.readFromValue(model_jval, b2world.get());

	vector<b2Body*> b2bodies;
	json.getAllBodies(b2bodies);

	for (b2Body* b2body : b2bodies)
	{
		Name name = json.getBodyName(b2body);
		shared_ptr<B2Body> body = make_shared<B2Body>(b2body, name);

		if (b2dJsonCustomProperties* properties = json.getCustomPropertiesForItem(b2body))
		{
			auto read = [&](auto& properties)
			{
				for (auto& [prop_name, value] : properties)
					Body::Class::GetProperty(prop_name)->SetValue(*body, value);
			};

			read(properties->m_customPropertyMap_string);
			read(properties->m_customPropertyMap_int);
			read(properties->m_customPropertyMap_float);
			read(properties->m_customPropertyMap_bool);
		}

		bodies.emplace_back(move(body));
	}

	vector<b2Joint*> b2joints;
	json.getAllJoints(b2joints);
	for (b2Joint* b2joint : b2joints)
	{
		Name name = json.getJointName(b2joint);
		joints.emplace_back(make_shared<B2Joint>(b2joint, name));
	}

	return {move(bodies), move(joints)};
}

string B2World::SaveToJson()
{
	b2dJson b2json;

	for (b2Body* b2body = b2world->GetBodyList(); b2body; b2body = b2body->GetNext())
	{
		B2Body* body = b2body->GetUserData();
		b2json.setBodyName(b2body, *body->name);
		b2json.setCustomString(b2body, "ObjectName", *body->object->name);
	}

	return b2json.writeToString(b2world.get());
}

void B2World::LoadFromJson(string_view json)
{
	b2dJson b2json;

	Json::Value jval;
	Json::Reader reader;
	reader.parse(json.data(), json.data() + json.size(), jval);

	b2json.readFromValue(jval, b2world.get());

	vector<b2Body*> bodies;
	b2json.getAllBodies(bodies);

	for (b2Body* b2body : bodies)
	{
		Name obj_name = b2json.getCustomString(b2body, "ObjectName"s);
		RigidObject* obj = FindObject(obj_name);
		if (!obj) obj = AddObject(make_shared<RigidObject>(obj_name));
		Name body_name = b2json.getBodyName(b2body);
		obj->AddPart(make_shared<B2Body>(b2body, body_name));
	}
}

void B2World::DebugDraw(const IDebugDrawer& drawer)
{
	B2Draw b2_drawer(drawer, b2Draw::e_shapeBit | b2Draw::e_centerOfMassBit /*| b2Draw::e_jointBit*/);
	b2world->SetDebugDraw(&b2_drawer);
	b2world->DebugDraw();
}

Vec2 B2World::GetGravity()
{
	return b2world->GetGravity();
}

void B2World::BeginContact(b2Contact* contact)
{
	if (!begin_contact(contact)) contact->SetEnabled(false);
}

void B2World::EndContact(b2Contact* contact)
{
	end_contact(contact);
}

void B2World::PreSolve(b2Contact* contact, const b2Manifold* old_manifold)
{
	if (!pre_solve(contact, old_manifold)) contact->SetEnabled(false);
}

void B2World::PostSolve(Contact* contact, const b2ContactImpulse* impulse)
{
	post_solve(contact, impulse);
}

B2World::~B2World()
{
	for (b2Body* b2body = b2world->GetBodyList(); b2body; b2body = b2body->GetNext())
		b2body->GetUserData()->b2body = nullptr;
}
