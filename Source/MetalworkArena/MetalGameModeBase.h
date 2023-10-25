#pragma once

#include "CoreEngine.h"
#include "MetalGameModeBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);

UCLASS()
class AMetalworkArenaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	TUniquePtr<MetalCore> Core;
	TSet<class AArenaActor*> actors;

public:
	AMetalworkArenaGameModeBase() { PrimaryActorTick.bCanEverTick = true; }
	void InitGame(const FString& map_name, const FString& options, FString& error) override;
	void Tick(float dt) override;
};