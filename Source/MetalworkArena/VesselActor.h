#pragma once

#include "ArenaActor.h"
#include "VesselActor.generated.h"

UCLASS()
class METALWORKARENA_API AVesselActor : public AArenaActor
{
	GENERATED_BODY()

protected:
	class Vessel* GetRig() { return (Vessel*)Rig; }
};

UCLASS()
class METALWORKARENA_API AChainActor : public AArenaActor
{
	GENERATED_BODY()

public:
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> ChainSound;

	bool Loose = false;

public:
	void BeginPlay() override;
	class Chain* GetRig() { return (Chain*)Rig; }
	void ArenaTick(float DeltaTime) override;
};