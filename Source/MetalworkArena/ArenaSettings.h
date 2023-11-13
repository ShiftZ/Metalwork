#pragma once

#include "RigidWorld.h"
#include "ArenaSettings.generated.h"

UCLASS()
class AArenaSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	unique_ptr<RigidWorld> Rig;

protected:
	void PostInitProperties() override;
	void PostLoad() override;
	void Serialize(FArchive& Ar) override;
};

UCLASS()
class AArenaScript : public ALevelScriptActor
{
	GENERATED_BODY()
};