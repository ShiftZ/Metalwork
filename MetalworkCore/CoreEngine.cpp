#include "Network.h"

#include <future>
#include <ranges>
#include "std.h"
#include "logger.h"

#include "CoreDefinitions.h"
#include "CoreEngine.h"

#ifdef _WIN32
#	include "WindowsInput.h"
#endif

MetalCore::MetalCore(int player, unique_ptr<Network> network_)
	: network(move(network_)), player(player), arena(1.f / fps)
{
	packaged_task init([&]
	{
		if constexpr (windows)
			input = make_unique<WindowsInput>();

		inputs.resize(network ? network->NumPeers() + 1 : 1);
	});

	thread = jthread([=, &init](stop_token st)
	{
		init();
		MainLoop(st);
	});

	init.get_future().get();
}

void MetalCore::Ready()
{
	if (network) network->SendTCP(ReadyMsg());
}

void MetalCore::ProcessMessages()
{
	while (network->HasMessages())
	{
		auto [sender, msg] = network->NextMessage();
		Visitor visitor
		{
			[&](AllReady&)
			{
				auto countdown_duration = 1s;
				if (player == 0)
					network->BeginCountdown(countdown_duration);
				start_time = steady_clock::now() + countdown_duration;
			},
			[&](CountdownMsg& countdown_msg)
			{
				int64_t start_in = countdown_msg.msecs_left;
				if (start_time != steady_clock::time_point())
				{
					int64_t msecs_left = duration_cast<milliseconds>(steady_clock::now() - start_time).count();
					if (msecs_left < 0) return;
					start_in = (start_in * countdown_msg.num + msecs_left) / (countdown_msg.num + 1);
				}

				start_time = steady_clock::now() + milliseconds(start_in);
			},
			[&](InputMsg& input_msg)
			{
				auto& [clean_seqid, pointer, _] = inputs[sender];
				pointer.try_emplace(pointer.end(), input_msg.seq_id, input_msg.step, input_msg.pointer);

				// move clean boundry
				auto clean_it = pointer.find(clean_seqid) + 1;
				while (clean_it != pointer.end() && clean_it->first == clean_seqid + 1)
				{
					++clean_seqid;
					++clean_it;
				}
			},
			[](auto&){ throw logic_error("No processor for message"); }
		};

		visit(visitor, msg);
	}
}

void MetalCore::MainLoop(stop_token st)
{
	while (start_time == steady_clock::time_point())
	{
		network->WaitMessage(st);
		if (st.stop_requested()) return;
		ProcessMessages();
	}

	while (start_time > steady_clock::now())
	{
		network->WaitMessage(st, start_time);
		if (st.stop_requested()) return;
		ProcessMessages();
	}

	input->Start();
	arena.Start();

	arena_thread = jthread([&](stop_token st)
	{
		for (;;)
		{
			decltype(arena_inputs) input;

			unique_lock input_lock(arena_input_mtx);
			arena_cv.wait(input_lock, st, [&]
			{
				if (arena_inputs.empty()) return false;
				input = move(arena_inputs);
				return true;
			});
			if (st.stop_requested()) return;

			unique_lock step_lock(arena_step_mtx);
			for (auto& [clean_input, dirty_input] : input)
				arena.Step({move(clean_input), move(dirty_input)});
		}
	});

	while (!st.stop_requested())
	{
		steady_clock::time_point next_step_time = start_time + milliseconds((step + 1) * (1000 / fps));

		do // wait step time, send inputs
		{
			input->WaitInput(st, next_step_time);
			if (st.stop_requested()) return;

			while (optional<Input> in = input->GetInput())
			{
				auto& [seq_id, pointer, _] = inputs[player];
				pointer.try_emplace(pointer.end(), ++seq_id, step, in->pointer);
				network->SendSeqUDP(InputMsg{seq_id, step, in->pointer});
			}
		}
		while (steady_clock::now() < next_step_time);

		// commit zero input
		auto& [seq_id, pointer, _] = inputs[player];
		if (auto& [id, pin] = pointer.back(); pin.step < step)
		{
			vec2i pos = input->pointer; // just make sure it stays the same
			pointer.try_emplace(pointer.end(), ++seq_id, step, pos);
			network->SendSeqUDP(InputMsg{seq_id, step, pos});
		}

		ProcessMessages();

		// find clean step
		int clean_step = this->step;
		for (auto& [clean_seqid, pointer_in, _] : inputs)
			clean_step = min(clean_step, pointer_in[clean_seqid].step - 1);

		if (player == 1)
		{
			++step; continue;
		}

		// copy input to arena thread
		{
			unique_lock lock(arena_input_mtx);
			auto& [clean_input, dirty_input] = arena_inputs.emplace_back();

			for (int player = 0; auto& [clean_seqid, pointer_in, clean_pointer] : inputs)
			{
				auto clean_it = ranges::upper_bound(pointer_in, clean_step, less(), [](value<PointerInput> in){ return in->step; });

				auto clean_range = ranges::subrange(pointer_in.begin(), clean_it) | views::values;
				auto dirty_range = ranges::subrange(clean_it, pointer_in.end()) | views::values;
				auto chunk_by_step = views::chunk_by([](PointerInput& a, PointerInput& b){ return a.step == b.step; });

				vec2i pointer = clean_pointer;

				for (size_t sub_step = 0; auto step_inputs : clean_range | chunk_by_step)
				{
					clean_input.resize(max(sub_step + 1, clean_input.size()));
					for (PointerInput& pointer_in : step_inputs)
					{
						clean_input[sub_step][player].move += (pointer_in.position - pointer) * 5;
						pointer = pointer_in.position;
					}
					++sub_step;
				}

				for (size_t sub_step = 0; auto step_inputs : dirty_range | chunk_by_step)
				{
					dirty_input.resize(max(sub_step + 1, dirty_input.size()));
					for (PointerInput& pointer_in : step_inputs)
					{
						dirty_input[sub_step][player].move += (pointer_in.position - pointer) * 5;
						pointer = pointer_in.position;
					}
					++sub_step;
				}

				if (clean_it != pointer_in.begin())
				{
					clean_pointer = value(*(clean_it - 1))->position;
					pointer_in.erase(pointer_in.begin(), clean_it); // discard clean input
				}
				++player;
			}

			arena_cv.notify_one();
		}

		++step;
	}
}

MetalCore::~MetalCore()
{
	thread.request_stop();
	arena_thread.request_stop();
}
