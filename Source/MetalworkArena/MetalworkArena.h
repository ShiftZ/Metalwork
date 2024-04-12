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

#include <Chaos/Adapters/CacheAdapter.h>
#include <Chaos/CacheCollection.h>
#include <ChaosClothAsset/ClothComponent.h>
#include <Chaos/ChaosCache.h>
#include <Components/AudioComponent.h>
#include <Components/InputComponent.h>
#include <DrawDebugHelpers.h>
#include <Engine/DataAsset.h>
#include <Engine/DataTable.h>
#include <Engine/DeveloperSettings.h>
#include <Engine/Engine.h>
#include <Engine/GameInstance.h>
#include <Engine/LevelScriptActor.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <Features/IModularFeature.h>
#include <GameFramework/Actor.h>
#include <GameFramework/GameModeBase.h>
#include <GameFramework/PlayerInput.h>
#include <HAL/FileManager.h>
#include <Kismet/GameplayStatics.h>
#include <MetasoundExecutableOperator.h>
#include <MetasoundFacade.h>
#include <MetasoundNodeRegistrationMacro.h>
#include <MetasoundParamHelper.h>
#include <MetasoundPrimitives.h>
#include <MetasoundSource.h>
#include <MetasoundVertex.h>
#include <MetasoundWave.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>
#include <Modules/ModuleManager.h>
#include <PaperFlipbookComponent.h>
#include <PaperSpriteComponent.h>
#include <Sound/SoundCue.h>
#include <TimerManager.h>
#include <UObject/LinkerLoad.h>
#include <UObject/Package.h>
#include <UObject/UObjectIterator.h>
#include <Misc/MessageDialog.h>
#include <Algo/FindLast.h>
#include <Logging/MessageLog.h>
#include <Engine/GameViewportClient.h>
#include <Algo/RemoveIf.h>
#include <NiagaraSystem.h>
#include <NiagaraComponent.h>

#if WITH_EDITOR
#include <DirectoryWatcherModule.h>
#include <IDirectoryWatcher.h>
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
#include "Name.h"

inline const FName SpeedName = "Speed";
inline const FName ImpulseName = "Impulse";
inline const FName AccelName = "Acceleration";

inline constexpr float ScaleRigToUE = 100.f;

inline struct UEScaler {} UEScale;
inline float operator*(float F, const UEScaler&) { return F * ScaleRigToUE; }
inline float operator/(float F, const UEScaler&) { return F / ScaleRigToUE; }
inline FVector operator*(const Vec2& P, const UEScaler& S) { return FVector(P.x * S, 0, P.y * S); }
inline Vec2 operator/(const FVector& P, const UEScaler& S) { return Vec2(P.X / S, P.Z / S); }


#if !UE_BUILD_SHIPPING
#	define Assert(Expression, ...) \
		(LIKELY(Expression) || DispatchCheckVerify<bool>([&]() UE_DEBUG_SECTION { \
			static std::chrono::system_clock::time_point TimeStamp; \
			if (std::chrono::system_clock::now() - TimeStamp < std::chrono::seconds(1)) return true; \
			int L = 0; \
			UE_DEBUG_BREAK(); \
			AssertionFailed(#Expression, __FILE__, __LINE__, PrintMessage(L##__VA_ARGS__)); \
			TimeStamp = std::chrono::system_clock::now(); \
			return false; \
		}))

void AssertionFailed(const char* Expr, const char* Loc, int Line, const FString& Message);
FString VARARGS PrintMessage(const TCHAR* Format, ...);
inline FString PrintMessage(int) { return {}; }

#else
#	define Assert(...) true
#endif


