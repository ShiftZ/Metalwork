// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <span>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <ranges>
#include <chrono>
#include <typeindex>

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
#include "tools/vector2.h"
#include "tools/vector3.h"
#include "tools/queue.h"
#include "tools/logger.h"

using namespace std;
using namespace std::chrono;
namespace views = std::views;