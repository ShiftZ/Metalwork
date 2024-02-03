#pragma once

#include "RigidWorld.h"
#include "EditorTicker.h"
#include "ArenaSettings.generated.h"

UCLASS()
class AArenaSettings : public AWorldSettings, public IEditorTickable
{
	GENERATED_BODY()

public:
	TUniquePtr<RigidWorld> RigWorld;

protected:
	void PostInitProperties() override;
	void Serialize(FArchive& Ar) override;
	void PostLoad() override;
	void EditorTick() override;
};

UCLASS()
class AArenaScript : public ALevelScriptActor
{
	GENERATED_BODY()
};