#include "Arena.h"

#include "Vessel.h"
#include "Weapon.h"
#include "Body.h"
#include "B2Rig/B2World.h"
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

unique_ptr<RigidWorld> Arena::MakeWorld()
{
	return make_unique<B2World>(-1);
}
