// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetalworkArena.h"
#include "CoreInterface.h"

class FMetalworkArenaModule : public FDefaultGameModuleImpl
{
	void StartupModule() override
	{
		SetContentPath(*FPaths::ProjectContentDir(), WITH_EDITOR);
		FCoreUObjectDelegates::ReloadCompleteDelegate.AddLambda([](EReloadCompleteReason)
			{ SetContentPath(*FPaths::ProjectContentDir(), WITH_EDITOR); });
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FMetalworkArenaModule, MetalworkArena, "MetalworkArena");
