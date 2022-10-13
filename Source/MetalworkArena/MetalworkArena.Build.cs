// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MetalworkArena : ModuleRules
{
	public MetalworkArena(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Paper2D" });

		//PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		bool DebugBuild = (Target.Configuration == UnrealTargetConfiguration.Debug) ||
						  (Target.Configuration == UnrealTargetConfiguration.DebugGame);

		PublicIncludePaths.Add(ModuleDirectory + "../../../MetalworkCore");
		PublicIncludePaths.Add(ModuleDirectory + "../../../MetalworkCore/toolbox");
		if (DebugBuild)
			PublicAdditionalLibraries.Add(ModuleDirectory + "/../../MetalworkCore/x64/Debug/MetalworkCore.lib");
		else
			PublicAdditionalLibraries.Add(ModuleDirectory + "/../../MetalworkCore/x64/Release/MetalworkCore.lib");

		CppStandard = CppStandardVersion.Latest;

		bEnableExceptions = true;
		bUseRTTI = true;

		PublicDefinitions.AddRange(new string[] {"B2_USER_SETTINGS", "ASIO_SEPARATE_COMPILATION", "_WIN32_WINNT=0x0601"});
	}
}
