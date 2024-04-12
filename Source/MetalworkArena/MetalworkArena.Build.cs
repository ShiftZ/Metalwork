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
        
        PrivateDependencyModuleNames.AddRange(new string[] 
            { "Core", "CoreUObject", "Engine", "Paper2D", "InputCore", "DeveloperSettings", "Niagara", "AssetRegistry" });
        
        PrivateDependencyModuleNames.AddRange(new string[] 
            { "MetasoundFrontend", "MetasoundStandardNodes", "MetasoundGraphCore", "MetasoundEngine" });
        
        PrivateDependencyModuleNames.AddRange(new string[] 
            { "ChaosClothAssetEngine", "ChaosCloth" });
        
        PrivateDependencyModuleNames.Add("MetalworkCore");
        PrivateDependencyModuleNames.Add("Box2D");
        
        PrivateDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
        if (Target.Type == TargetType.Editor)
        {
            PrivateDefinitions.Add("LOGGER_IMPORT");
            PrivateDependencyModuleNames.Add("DirectoryWatcher");
        }
    }
}
