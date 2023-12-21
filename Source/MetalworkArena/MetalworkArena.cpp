// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreInterface.h"
#include "MetalworkArena.h"

class FMetalworkArenaModule : public FDefaultGameModuleImpl
{
	void StartupModule() override { SetContentPath(*FPaths::ProjectContentDir()); }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FMetalworkArenaModule, MetalworkArena, "MetalworkArena");
