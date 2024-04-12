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

UCLASS()
class UArenaEvent : public UObject
{
	GENERATED_BODY()

public:
	ArenaEvent* CoreEvent;

public:
	virtual void Confirmed() {}
	virtual void Denied() {}
	virtual ~UArenaEvent() = default;
};

UCLASS()
class USoundEvent : public UArenaEvent
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<UAudioComponent> Component;
	void Denied() override { if (Component.IsValid()) Component->DestroyComponent(); }
};

UCLASS()
class UEffectEvent : public UArenaEvent
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<UNiagaraComponent> Component;
	void Denied() override { if (Component.IsValid()) Component->DestroyComponent(); }
};