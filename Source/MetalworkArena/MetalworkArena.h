// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <any>
#include <chrono>
#include <filesystem>
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

#include <Components/InputComponent.h>
#include <DrawDebugHelpers.h>
#include <Engine/DataTable.h>
#include <Engine/Engine.h>
#include <Engine/GameInstance.h>
#include <Engine/LevelScriptActor.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <GameFramework/Actor.h>
#include <GameFramework/GameModeBase.h>
#include <GameFramework/PlayerInput.h>
#include <HAL/FileManager.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>
#include <Modules/ModuleManager.h>
#include <PaperFlipbookComponent.h>
#include <PaperSpriteComponent.h>
#include <TimerManager.h>
#include <UObject/LinkerLoad.h>
#include <UObject/Package.h>

#include "tools/flat_map.h"
#include "tools/logger.h"
#include "tools/queue.h"
#include "tools/sol_hash_set.h"
#include "tools/std.h"
#include "tools/vector2.h"
#include "tools/vector3.h"

using namespace std;
using namespace std::chrono;
using namespace std::filesystem;
namespace views = std::views;

inline constexpr float ScaleRigToUE = 100.f;

inline struct UEScaler {} UEScale;
inline float operator*(float F, const UEScaler& S) { return F * ScaleRigToUE; }
inline float operator/(float F, const UEScaler& S) { return F / ScaleRigToUE; }
inline FVector operator*(const vec2& P, const UEScaler& S) { return FVector(P.x * S, 0, P.y * S); }
inline vec2 operator/(const FVector& P, const UEScaler& S) { return vec2(P.X / S, P.Z / S); }