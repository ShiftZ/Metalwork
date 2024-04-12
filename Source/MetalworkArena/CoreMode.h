#pragma once

#include "CoreEngine.h"
#include "CoreMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);

UCLASS()
class ACoreMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	TUniquePtr<MetalCore> Core;
	int Player = 0, Players = 1;

	UPROPERTY()
	TArray<class UArenaEvent*> Events;

	UPROPERTY()
	class USoundData* SoundData;

	UPROPERTY()
	class UEffectsData* EffectsData;

public:
	ACoreMode() { PrimaryActorTick.bCanEverTick = true; }
	void BeginPlay() override;
	void InitGame(const FString& MapName, const FString& Options, FString& Error) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	void Pause();
	void Tick(float DeltaTime) override;
	void ProcessEvents();
};

