#pragma once

#pragma warning(disable:4251)

#include "Arena.h"
#include "CoreInterface.h"

class METALWORKCORE_API MetalCore : public IMetalCore
{
	struct PointerInput
	{
		int step;
		vec2i position;
	};

	struct PlayerInput
	{
		int clean_seqid = 0;
		flat_map<int, PointerInput> pointer = {{0, {0, 0}}};
		vec2i clean_pointer = 0;
	};

private:
	unique_ptr<class InputInterface> input;
	unique_ptr<class Network> network;
	int player;
	int step = 0;
	steady_clock::time_point start_time;

	vector<PlayerInput> inputs;
	
	jthread arena_thread;
	mutex arena_input_mtx, arena_step_mtx;
	condition_variable_any arena_cv;
	vector<Arena::StepInputs> arena_inputs;

	jthread thread;

public:
	Arena arena;

private:
	void ProcessMessages();
	void GetReady(stop_token st);
	void MainLoop(stop_token st);

public:
	MetalCore(int player, unique_ptr<RigidWorld> world, unique_ptr<INetwork> net);
	~MetalCore() override;

	Arena& Arena() { return arena; }

	void Ready() override;
};