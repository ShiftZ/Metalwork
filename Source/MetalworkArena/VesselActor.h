#pragma once

#include "ArenaActor.h"
#include "VesselActor.generated.h"


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

