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

public:
	AMetalworkArenaGameModeBase() { PrimaryActorTick.bCanEverTick = true; }
	void BeginPlay() override;
	void InitGame(const FString& MapName, const FString& Options, FString& Error) override;
	void Pause();
	void Tick(float DeltaTime) override;
};