#pragma once

#include "ArenaActor.h"
#include "VesselActor.generated.h"

UCLASS()
class METALWORKARENA_API AVesselActor : public AArenaActor
{
	GENERATED_BODY()

protected:
	class Vessel* GetRig() { return (Vessel*)Rig; }
	void ArenaTick(float DeltaTime) override;
};

UCLASS()
class METALWORKARENA_API AChainActor : public AArenaActor
{
	GENERATED_BODY()

public:
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ChainSound;

	Float LastRopeForce = 0;
	Float ChainAngle = 0;
	Float AngleChangeSpeed = 0;
	bool Charged = false;

public:
	void BeginPlay() override;
	class Chain* GetRig() { return (Chain*)Rig; }
	void ArenaTick(float DeltaTime) override;
};