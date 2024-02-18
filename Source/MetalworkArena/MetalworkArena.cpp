// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetalworkArena.h"
#include "CoreInterface.h"
#include "MetasoundFrontendRegistries.h"

class FMetalworkArenaModule : public FDefaultGameModuleImpl
{
	void StartupModule() override
	{
		SetContentPath(*FPaths::ProjectContentDir(), WITH_EDITOR);

		FCoreUObjectDelegates::ReloadCompleteDelegate.AddLambda([](EReloadCompleteReason) {
			SetContentPath(*FPaths::ProjectContentDir(), WITH_EDITOR);
		});
		FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FMetalworkArenaModule, MetalworkArena, "MetalworkArena");
