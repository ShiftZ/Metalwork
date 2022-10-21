#pragma once

#include <chrono>
#include <unordered_set>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoreEngine.h"

#include "MetalGameModeBase.generated.h"

using namespace std::chrono;

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);

UCLASS()
class METALWORKARENA_API AMetalworkArenaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	unique_ptr<MetalCore> core;
	unordered_set<class AArenaActor*> actors;

public:
	AMetalworkArenaGameModeBase() { PrimaryActorTick.bCanEverTick = true; }
	void InitGame(const FString& map_name, const FString& options, FString& error) override;
	void Tick(float dt) override;
};