// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MetalworkArena : ModuleRules
{
	public MetalworkArena(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "MetalworkCore.h";

        CppStandard = CppStandardVersion.Latest;
        bEnableExceptions = true;
        bUseRTTI = true;

        PrivatePCHHeaderFile = "MetalworkArena.h";
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Paper2D", "MetalworkCore" });

        if (Target.Type == TargetType.Editor) PrivateDefinitions.Add("LOGGER_IMPORT");
    }
}
