#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <condition_variable>
#include "flat_map.h"

#include "Arena.h"

using namespace std;
using namespace std::chrono;

class MetalCore
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
	};

private:
	unique_ptr<class InputInterface> input;
	unique_ptr<class Network> network;
	int player;
	int step = 0;
	steady_clock::time_point start_time;

	vector<PlayerInput> inputs;

	Arena arena;
	jthread arena_thread;
	mutex arena_input_mtx;
	condition_variable_any arena_cv;
	vector<array<vector<vector<Arena::PlayerInput>>, 2>> arena_input; 

	jthread thread;

private:
	void ProcessMessages();
	void MainLoop(stop_token st);

public:
	MetalCore(int player, unique_ptr<Network> network);
	void Ready();
	~MetalCore();
};