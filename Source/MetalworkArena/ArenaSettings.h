#pragma once

#include "RigidWorld.h"
#include "EditorTicker.h"
#include "ArenaSettings.generated.h"

UCLASS()
class AArenaSettings : public AWorldSettings, public IEditorTickable
{
	GENERATED_BODY()

	static inline FString WatchDirectory = FPaths::ProjectContentDir() + L"Props";

public:
	TUniquePtr<RigidWorld> RigWorld;
	FDelegateHandle WatchHandle;

protected:
	void PostInitProperties() override;
	void PostLoad() override;
	void EditorTick() override;
	~AArenaSettings();
};

UCLASS()
class AArenaScript : public ALevelScriptActor
{
	GENERATED_BODY()
};