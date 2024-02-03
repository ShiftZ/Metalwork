#pragma once

#include "ArenaActor.h"
#include "VesselActor.generated.h"

UCLASS()
class METALWORKARENA_API AVesselActor : public AArenaActor
{
	GENERATED_BODY()

public:
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> PropellerSound;

protected:
	void BeginPlay() override;
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
	TObjectPtr<USoundBase> ChainSound;

	bool Loose = false;

public:
	void BeginPlay() override;
	class Chain* GetRig() { return (Chain*)Rig; }
	void ArenaTick(float DeltaTime) override;
};

UCLASS()
class METALWORKARENA_API AAnchorActor : public AArenaActor
{
	GENERATED_BODY()

public:
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> WhooshSound;

public:
	void BeginPlay() override;
	class Anchor* GetRig() { return (Anchor*)Rig; }
	void ArenaTick(float DeltaTime) override;
};