#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <numbers>
#include <numeric>
#include <ranges>
#include <thread>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <span>
#include <typeindex>

#undef JSON_API
#include <json/json.h>

#include <box2d/b2_body.h>
#include <box2d/b2_distance_joint.h>
#include <box2d/b2_revolute_joint.h>
#include <box2d/b2_world.h>

#undef TEXT
#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>
#include <asio/cancellation_signal.hpp>
#include <asio/io_context.hpp>
#include <asio/read.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/experimental/awaitable_operators.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>

#include "tools/flat_map.h"
#include "tools/logger.h"
#include "tools/queue.h"
#include "tools/std.h"
#include "tools/vector2.h"
#include "tools/vector3.h"

using namespace std;
namespace views = std::ranges::views;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::filesystem;
using namespace std::string_literals;

using namespace asio;
using namespace asio::ip;
using namespace asio::experimental;
using namespace asio::experimental::awaitable_operators;

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#endif
