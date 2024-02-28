#pragma once

#include "RigidWorld.h"
#include "ArenaActor.generated.h"

UCLASS()
class METALWORKARENA_API AArenaActor : public AActor, public MetalActor
{
	GENERATED_BODY()

protected:
	RigidObject* Rig = nullptr;

public:
	void AttachToRig(RigidObject* Rig);
	virtual void ArenaTick(float DeltaTime);

protected:
	void OnConstruction(const FTransform& Transform) override;
	void Destroyed() override;
	void PostEditMove(bool bFinished) override;

	void ReleaseRig() override { Rig = nullptr; }
	void SyncPose();
};

UINTERFACE(MinimalAPI)
class UModelPart : public UInterface
{
	GENERATED_BODY()
};

class IModelPart
{
    GENERATED_BODY()

public:
	class Body* Body = nullptr;
};

UCLASS(Blueprintable)
class METALWORKARENA_API UMyFlipbookComponent : public UPaperFlipbookComponent, public IModelPart
{
	GENERATED_BODY()
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class METALWORKARENA_API UMySpriteComponent : public UPaperSpriteComponent, public IModelPart
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
