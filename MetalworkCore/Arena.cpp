#include <ranges>

#include "CoreDefinitions.h"
#include "Vehicle.h"
#include "B2World.h"
#include "Weapon.h"

#include "toolbox/logger.h"

#include "Arena.h"

namespace views = std::ranges::views;

Arena::Arena(float step_time)
{
	rigid_world = make_unique<B2World>(step_time, -5);

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

void Arena::Start()
{
	rigid_world->Capture();
}

void Arena::Step(StepInputs inputs)
{
	rigid_world->Restore();

	unique_lock lock(step_mtx);

	if (!inputs.clean.empty())
	{
		log(DisplayLog{1}, "Clean size: {}", inputs.clean.size());

		for (flat_map<int, PlayerInput>& players_input : inputs.clean)
		{
			for (auto& [player, input] : players_input)
			{
				Vehicle* vehicle = vehicles[player].get();
				vehicle->body->root->ApplyForce(input.move);
				++player;
			}

			rigid_world->Step();
		}

		rigid_world->Capture();
	}

	for (flat_map<int, PlayerInput>& players_input : inputs.dirty)
	{
		log(DisplayLog{2}, "Dirty size: {}", inputs.dirty.size());

		for (auto& [player, input] : players_input)
		{
			Vehicle* vehicle = vehicles[player].get();
			vehicle->body->root->ApplyForce(input.move);
			++player;
		}

		rigid_world->Step();
	}

	rigid_world->Restore();
}