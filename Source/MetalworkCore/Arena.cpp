#include "Arena.h"

#include "Vessel.h"
#include "Weapon.h"
#include "Body.h"
#include "B2Rig/B2World.h"
#include "B2Rig/B2Contact.h"
#include "Contact.h"
#include "CoreInterface.h"

Arena::Arena(unique_ptr<RigidWorld> rigid_world)
{
	Vessel* player1 = rigid_world->AddObject(make_shared<Vessel>(0, "player1"));
	player1->LoadModel(GetJson("Vessels/Dummy"));

	Chain* chain1 = rigid_world->AddObject(make_shared<Chain>("chain1"));
	chain1->LoadModel(GetJson("Weapons/Chain"));

	Anchor* ball1 = rigid_world->AddObject(make_shared<Anchor>("ball1"));
	ball1->LoadModel(GetJson("Weapons/Ball"));

	chain1->AttachAnchor(ball1);
	player1->AttachWeapon(chain1);
	player1->SetPosition({-2, 4});

	vessels.push_back(player1);

	Vessel* player2 = rigid_world->AddObject(make_shared<Vessel>(1, "player2"));
	player2->LoadModel(GetJson("vessels/dummy"));

	/*Weapon* weapon2 = rigid_world->AddObject(make_shared<Weapon>("weapon2"));
	weapon2->LoadModel(GetJson("weapons/chain-ball"));
	player2->AttachWeapon(weapon2); */
	player2->SetPosition({2, 0});
	
	vessels.push_back(player2);

	rigid_world->begin_contact = bind(&Arena::BeginContact, this, placeholders::_1);
	rigid_world->end_contact = bind(&Arena::EndContact, this, placeholders::_1);
	rigid_world->pre_solve = bind(&Arena::PreSolve, this, placeholders::_1, placeholders::_2);
	rigid_world->post_solve = bind(&Arena::PostSolve, this, placeholders::_1, placeholders::_2);

	this->rigid_world = move(rigid_world);
}

void Arena::Start()
{
	rigid_world->Capture();
}

void Arena::Step(StepInputs inputs)
{
	rigid_world->Restore();

	unique_lock lock(step_mtx);

	auto one_step = [&]
	{
		rigid_world->Step();

		for (auto& [body, impulses] : snapped_bodies)
		for (auto& [impulse, point] : impulses)
		{
			body->SetMovablity(Movability::Dynamic);
			body->ApplyImpulse(impulse, point);
			body->role = Body::None;
		}

		snapped_bodies.resize(0);
	};

	for (ArenaEvent* event : events | cptr)
		event->step = numeric_limits<int>::max();

	// clean step
	for (flat_map<int, PlayerInput>& players_input : inputs.clean)
	{
		for (auto& [player, input] : players_input)
		{
			vessels[player]->SetPlayerInput(input.move);
		}

		one_step();
	}

	// commit clean events
	for (unique_ptr<ArenaEvent>& event : events)
		if (event->step <= rigid_world->step)
			events_confirmed.push_back(move(event));

	erase(events, nullptr);

	rigid_world->Capture();

	// dirty step
	for (flat_map<int, PlayerInput>& players_input : inputs.dirty)
	{
		for (auto& [player, input] : players_input)
			vessels[player]->root->ApplyForce(input.move);

		one_step();
	}

	// deny unconfirmed events
	for (unique_ptr<ArenaEvent>& event : events)
		if (event->step == numeric_limits<int>::max()) 
			events_denied.push_back(move(event));

	erase(events, nullptr);
}

bool Arena::CheckOutEvent(Contact* contact, const type_info* type)
{
	auto event = ranges::find_if(events, [&](unique_ptr<ArenaEvent>& event){ return event->contact == contact && &typeid(*event) == type; });
	if (event == events.end()) return false;
	(*event)->step = rigid_world->step;
	return true;
}

template <typename EventType, typename... ArgTypes>
EventType* Arena::MakeEvent(Contact* contact, ArgTypes&&... args)
{
	const type_info* type = &typeid(EventType);
	auto found = ranges::find_if(events, [&](unique_ptr<ArenaEvent>& event){ return event->contact == contact && &typeid(*event) == type; });
	if (found == events.end())
	{
		unique_ptr event = make_unique<EventType>(rigid_world->step, contact, forward<ArgTypes>(args)...);
		return (EventType*)events.emplace_back(move(event)).get();
	}
	(*found)->step = rigid_world->step;
	return nullptr;
}

bool Arena::BeginContact(Contact* contact)
{
	auto [bodyA, bodyB] = GetBodies(contact);

	return true;
}

void Arena::EndContact(Contact* contact)
{
}

bool Arena::PreSolve(Contact* contact, const void* data)
{
	auto [bodyA, bodyB] = GetBodies(contact);

	return true;
}

void Arena::PostSolve(Contact* contact, const void* data)
{
	auto [bodyA, bodyB] = GetBodies(contact);

	Float impulse = GetNormalImpulseMag(contact, data);

	auto do_body = [&](Body* body, Body* other)
	{
		if (body->role == Body::Prop)
		{
			if (impulse > body->snap_impulse)
				snapped_bodies.emplace_back(body, GetImpulses(contact, data, body));
		}

		if (body->role == Body::Prop && &typeid(*other->object) == &typeid(Anchor) && impulse > 1.0f)
		{
			if (auto* event = MakeEvent<ImpactEvent>(contact, impulse))
			{
				events_begun.push_back(event);
			}
		}
	};

	do_body(bodyA, bodyB), do_body(bodyB, bodyA);
}

unique_ptr<RigidWorld> Arena::MakeWorld()
{
	return make_unique<B2World>(-1);
}
