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
	TSet<class AArenaActor*> Actors;

public:
	AMetalworkArenaGameModeBase() { PrimaryActorTick.bCanEverTick = true; }
	void InitGame(const FString& MapName, const FString& Options, FString& Error) override;
	void Tick(float DeltaTime) override;
};