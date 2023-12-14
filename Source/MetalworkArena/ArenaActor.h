#pragma once

#include "RigidWorld.h"
#include "ArenaActor.generated.h"

UCLASS()
class METALWORKARENA_API AArenaActor : public AActor, public MetalActor
{
	GENERATED_BODY()

public:
	RigidObject* Rig = nullptr;

public:
	AArenaActor() { PrimaryActorTick.bCanEverTick = true; }
	void AttachToRig(RigidObject* Rig);
	void SyncPose();
	void Tick(float dt) override;
};

UCLASS()
class METALWORKARENA_API APropActor : public AArenaActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(GetOptions="GetRigs"))
	FName RigModel;

	UFUNCTION()
	TArray<FName> GetRigs();

	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	void PostLoad() override;
};

UINTERFACE(MinimalAPI)
class UComponentPocket : public UInterface
{
    GENERATED_BODY()
};

class IComponentPocket
{
    GENERATED_BODY()

public:
	class RigidBody* Body;
};

UCLASS(Blueprintable)
class METALWORKARENA_API UMyFlipbookComponent : public UPaperFlipbookComponent, public IComponentPocket
{
	GENERATED_BODY()
};

UCLASS(Blueprintable)
class METALWORKARENA_API UMySpriteComponent : public UPaperSpriteComponent, public IComponentPocket
{
	GENERATED_BODY()
};

USTRUCT()
struct FVehicleModelPartRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<USceneComponent> ComponentClass;
};
