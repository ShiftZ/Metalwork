#include "B2World.h"
#include "B2Body.h"
#include "CoreDefinitions.h"
#include "b2dJson/b2dJson.h"

B2World::B2World(float gravity)
{
	world = make_unique<b2World>(b2Vec2(0, gravity));
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
	
	memcpy(ptr, world.get(), sizeof(b2World));
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
	memcpy(world.get(), data_ptr, sizeof(b2World));
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
	world->Step((1000 / fps) * 0.001f, 16, 16);
	++step;
}

unordered_map<string, shared_ptr<RigidBody>> B2World::LoadModel(Json::Value& model)
{
	unordered_map<string, shared_ptr<RigidBody>> parts;

	b2dJson json;
	json.readFromValue(model, world.get());

	vector<b2Body*> bodies;
	json.getAllBodies(bodies);

	for (b2Body* body : bodies)
	{
		shared_ptr part = make_shared<B2Body>(body);
		part->model_name = json.getCustomString(body, "Model");
		parts.emplace(json.getBodyName(body), move(part));
	}

	return parts;
}

string B2World::SaveToJson()
{
	b2dJson b2json;

	for (b2Body* body = world->GetBodyList(); body; body = body->GetNext())
	{
		string name = body->GetUserData()->object->actor->GetName();
		b2json.setBodyName(body, name.c_str());
	}

	return b2json.writeToString(world.get());
}

void B2World::LoadFromJson(string_view json)
{
	b2dJson b2json;

	Json::Value jval;
    Json::Reader reader;
	reader.parse(json.data(), json.data() + json.size(), jval);

	b2json.readFromValue(jval, world.get());

	vector<b2Body*> bodies;
	b2json.getAllBodies(bodies);

	unordered_map<string, vector<b2Body*>> objects;
	for (b2Body* body : bodies)
		objects[b2json.getBodyName(body)].push_back(body);

	for (auto& [name, parts] : objects)
	{
		shared_ptr object = make_shared<RigidObject>();
		for (b2Body* body : parts)
		{

			object->parts
		}
	}
}

B2World::~B2World()
{
	for (b2Body* b2body = world->GetBodyList(); b2body; b2body = b2body->GetNext())
		b2body->GetUserData()->body = nullptr;
}
