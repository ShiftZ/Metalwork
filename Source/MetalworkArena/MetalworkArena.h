// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <any>
#include <chrono>
#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

#include <Engine/Engine.h>
#include <Engine/GameInstance.h>
#include <Engine/World.h>
#include <GameFramework/GameModeBase.h>
#include <Modules/ModuleManager.h>
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"

#include "tools/flat_map.h"
#include "tools/logger.h"
#include "tools/queue.h"
#include "tools/vector2.h"
#include "tools/vector3.h"

using namespace std;
using namespace std::chrono;
namespace views = std::views;