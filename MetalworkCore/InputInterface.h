#pragma once

#include <chrono>
#include <condition_variable>
#include "toolbox/vector2.h"
#include "toolbox/queue.h"

using namespace std;
using namespace std::chrono;

struct Input
{
	steady_clock::time_point time;
	vec2i pointer;
};

class InputInterface
{
	mutex mtx;
	condition_variable_any cv;

public:
	vec2i pointer = 0;
	queue<Input> inputs;

public:
	virtual void Start() {}

	void WaitInput(stop_token st, steady_clock::time_point until)
	{
		unique_lock lock(mtx);
		cv.wait_until(lock, st, until, [&]{ return !inputs.empty(); });
	}

	optional<Input> GetInput()
	{
		optional<Input> input;
		inputs.pop(input);
		return input;
	}

	virtual ~InputInterface() = default;
};