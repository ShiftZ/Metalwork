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

	// clean step
	for (int substep = 0; flat_map<int, PlayerInput>& players_input : inputs.clean)
	{
		for (auto& [player, input] : players_input)
		{
			vessels[player]->SetPlayerInput(input.move);
		}

		rigid_world->Step();
		++substep;
	}

	rigid_world->Capture();

	// dirty step
	for (flat_map<int, PlayerInput>& players_input : inputs.dirty)
	{
		for (auto& [player, input] : players_input)
			vessels[player]->root->ApplyForce(input.move);

		rigid_world->Step();
	}
}

bool Arena::BeginContact(Contact* contact)
{
	Body* bodyA = GetBodyA(contact);
	Body* bodyB = GetBodyB(contact);

	if (bodyA->role == Body::Prop || bodyB->role == Body::Prop)
		return false;

	return true;
}

void Arena::EndContact(Contact* contact)
{

}

bool Arena::PreSolve(Contact* contact, const void* data)
{
	Body* bodyA = GetBodyA(contact);
	Body* bodyB = GetBodyB(contact);

	if (bodyA->role == Body::Prop || bodyB->role == Body::Prop)
		return false;

	return true;
}

void Arena::PostSolve(Contact* contact, const void* data)
{
}

unique_ptr<RigidWorld> Arena::MakeWorld()
{
	return make_unique<B2World>(-1);
}