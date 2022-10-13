#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Vehicle.h"
#include "VehicleActor.generated.h"

UCLASS()
class METALWORKARENA_API AArenaActor : public AActor
{
	GENERATED_BODY()

	RigidObject* object = nullptr;

public:
	AArenaActor() { PrimaryActorTick.bCanEverTick = true; }
	void AttachToObject(RigidObject* object);
	void SyncPose();
	void Tick(float dt) override;
};

UCLASS()
class METALWORKARENA_API AVehicleActor : public AArenaActor, public VehicleActor
{
	GENERATED_BODY()

public:	
	AVehicleActor() { PrimaryActorTick.bCanEverTick = true; }
};

UCLASS()
class METALWORKARENA_API AWeaponActor : public AArenaActor
{
	GENERATED_BODY()

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
	RigidBody* body;
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
	TSubclassOf<USceneComponent> component_class;
};