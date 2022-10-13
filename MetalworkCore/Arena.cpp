#include "Vehicle.h"
#include "B2World.h"
#include "Weapon.h"

#include "Arena.h"


Arena::Arena(float step_time)
{
	rigid_world = make_unique<B2World>(step_time);

	shared_ptr<Vehicle> player1 = make_shared<Vehicle>(this, "dummy");
	shared_ptr<Weapon> weapon1 = make_shared<Weapon>(this, "chain-ball");
	player1->AttachWeapon(weapon1);
	player1->SetPosition({-25, 0});

	vehicles.push_back(move(player1));

	shared_ptr<Vehicle> player2 = make_shared<Vehicle>(this, "dummy");
	shared_ptr<Weapon> weapon2 = make_shared<Weapon>(this, "chain-ball");
	player2->AttachWeapon(weapon2);
	player2->SetPosition({25, 0});

	vehicles.push_back(move(player2));
}

void Arena::CleanStep(vector<vector<PlayerInput>> inputs)
{
	rigid_world->Restore();

	if (inputs.empty()) return;

	for (vector<PlayerInput>& players_input : inputs)
	{
		for (int player = 0; player < players_input.size(); ++player)
		{
			// apply players input
		}

		rigid_world->Step();
	}

	rigid_world->Capture();
}

void Arena::DirtyStep(vector<vector<PlayerInput>> inputs)
{
	if (inputs.empty()) return;

	for (vector<PlayerInput>& players_input : inputs)
	{
		for (int player = 0; player < players_input.size(); ++player)
		{
			// apply players input
		}

		rigid_world->Step();
	}
}