#pragma once

#include "RigidWorld.h"
#include "DebugDrawer.h"
#include "ArenaSettings.generated.h"

UCLASS()
class AArenaSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	TUniquePtr<RigidWorld> RigWorld;
	TUniquePtr<DebugDrawer> Drawer;

protected:
	void PostInitProperties() override;
	void Serialize(FArchive& Ar) override;
	void PostLoad() override;
};

UCLASS()
class AArenaScript : public ALevelScriptActor
{
	GENERATED_BODY()
};