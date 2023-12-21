#pragma once

#include "EditorTicker.generated.h"

UINTERFACE(MinimalAPI)
class UEditorTickable : public UInterface
{
    GENERATED_BODY()
};

class IEditorTickable
{
    GENERATED_BODY()

public:
    virtual void EditorTick() = 0;
};

UCLASS(Transient)
class AEditorTicker : public AActor
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;

public:
	AEditorTicker()
	{
		bAllowTickBeforeBeginPlay = true;
		PrimaryActorTick.bTickEvenWhenPaused = true;
		bIsEditorOnlyActor = true;
	}

    void OnConstruction(const FTransform& Transform) override
	{
		IEditorTickable* Tickable = Cast<IEditorTickable>(GetParentActor());
		auto TickFunc = [=]{ Tickable->EditorTick(); };
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TickFunc, 1/45.f, true);
	}
};