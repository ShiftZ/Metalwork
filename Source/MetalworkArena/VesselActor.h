#pragma once

#include "ArenaActor.h"
#include "VesselActor.generated.h"

UCLASS()
class METALWORKARENA_API AVesselActor : public AArenaActor
{
	GENERATED_BODY()

	float HeadJointForce = 0;

public:
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ChainSound;

protected:
	AVesselActor();
	class Vessel* GetRig() { return (Vessel*)Rig; }
	void ArenaTick(float DeltaTime) override;
};

UCLASS()
class METALWORKARENA_API AChainActor : public AArenaActor
{
	GENERATED_BODY()
};