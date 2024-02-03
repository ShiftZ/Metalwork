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
#include <numbers>
#include <ranges>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

#include <DrawDebugHelpers.h>
#include <EngineUtils.h>
#include <PaperFlipbookComponent.h>
#include <PaperSpriteComponent.h>
#include <TimerManager.h>
#include <Components/AudioComponent.h>
#include <Components/InputComponent.h>
#include <Engine/DataTable.h>
#include <Engine/DeveloperSettings.h>
#include <Engine/Engine.h>
#include <Engine/GameInstance.h>
#include <Engine/LevelScriptActor.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <GameFramework/GameModeBase.h>
#include <GameFramework/PlayerInput.h>
#include <HAL/FileManager.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>
#include <Modules/ModuleManager.h>
#include <Sound/SoundCue.h>
#include <UObject/LinkerLoad.h>
#include <UObject/Package.h>

#if WITH_EDITOR
#include <DirectoryWatcherModule.h>
#include <IDirectoryWatcher.h>

#include <MetasoundExecutableOperator.h>
#include <MetasoundFacade.h>
#include <MetasoundNodeRegistrationMacro.h>
#include <MetasoundParamHelper.h>
#include <MetasoundPrimitives.h>
#include <MetasoundVertex.h>
#endif

#include "tools/flat_map.h"
#include "tools/logger.h"
#include "tools/queue.h"
#include "tools/sol_hash_set.h"
#include "tools/std.h"
#include "tools/vector2.h"
#include "tools/math.h"

using namespace std;
using namespace std::chrono;
using namespace std::filesystem;
using namespace std::numbers;
namespace views = std::views;

#include "Vec2.h"

inline const FName SpeedName = "Speed";
inline const FName ImpulseName = "Impulse";
inline const FName AccelName = "Acceleration";

inline constexpr float ScaleRigToUE = 100.f;

inline struct UEScaler {} UEScale;
inline float operator*(float F, const UEScaler&) { return F * ScaleRigToUE; }
inline float operator/(float F, const UEScaler&) { return F / ScaleRigToUE; }
inline FVector operator*(const Vec2& P, const UEScaler& S) { return FVector(P.x * S, 0, P.y * S); }
inline Vec2 operator/(const FVector& P, const UEScaler& S) { return Vec2(P.X / S, P.Z / S); }