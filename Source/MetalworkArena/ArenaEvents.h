#pragma once

#include "ArenaEvents.generated.h"

UCLASS()
class UArenaEvent : public UObject
{
	GENERATED_BODY()

public:
	class ArenaEvent* CoreEvent;

public:
	virtual void Confirmed() {}
	virtual void Denied() {}
	virtual ~UArenaEvent() = default;
};

UCLASS()
class UEffectEvent : public UArenaEvent
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<UActorComponent> Component;
	TSharedPtr<FStreamableHandle> StreamHandle;

	void Denied() override;

	static UEffectEvent* MakeSound(UWorld* World, TSoftObjectPtr<UMetaSoundSource> Sound,
											 FVector Location, TOptional<TPair<FName, float>> Params = {});

	static UEffectEvent* MakeVFX(UWorld* World, TSoftObjectPtr<UNiagaraSystem> Template, FVector Location);
};