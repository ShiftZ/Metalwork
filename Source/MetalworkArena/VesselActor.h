#pragma once

#include "Vessel.h"
#include "VesselActor.generated.h"

UCLASS()
class METALWORKARENA_API AArenaActor : public AActor
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
class METALWORKARENA_API AVesselActor : public AArenaActor
{
	GENERATED_BODY()

public:	
	AVesselActor() { PrimaryActorTick.bCanEverTick = true; }
};

UCLASS()
class METALWORKARENA_API AWeaponActor : public AArenaActor
{
	GENERATED_BODY()

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

	void PostEditChangeProperty(FPropertyChangedEvent& Event);
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