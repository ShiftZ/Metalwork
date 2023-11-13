#include "Arena.h"

#include "Vessel.h"
#include "B2World.h"
#include "Weapon.h"

Arena::Arena()
{
	rigid_world = MakeWorld();

	shared_ptr<Vessel> player1 = make_shared<Vessel>(this, "dummy");
	shared_ptr<Weapon> weapon1 = make_shared<Weapon>(this, "chain-ball");
	player1->AttachWeapon(weapon1);
	player1->SetPosition({-25, 0});

	vessels.push_back(move(player1));

	shared_ptr<Vessel> player2 = make_shared<Vessel>(this, "dummy");
	shared_ptr<Weapon> weapon2 = make_shared<Weapon>(this, "chain-ball");
	player2->AttachWeapon(weapon2);
	player2->SetPosition({25, 0});

	vessels.push_back(move(player2));
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
			Vessel* vessel = vessels[player].get();
			vessel->body->root->ApplyForce(input.move);
		}

		rigid_world->Step();
		++substep;
	}

	rigid_world->Capture();

	// dirty step
	for (flat_map<int, PlayerInput>& players_input : inputs.dirty)
	{
		for (auto& [player, input] : players_input)
		{
			Vessel* vessel = vessels[player].get();
			vessel->body->root->ApplyForce(input.move);
		}

		rigid_world->Step();
	}
}

unique_ptr<RigidWorld> Arena::MakeWorld()
{
	return make_unique<B2World>(-5);
}
