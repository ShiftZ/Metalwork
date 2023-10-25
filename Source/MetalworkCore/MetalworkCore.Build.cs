using UnrealBuildTool;
using System;
using System.IO;

public class MetalworkCore : ModuleRules
{
    public MetalworkCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "MetalworkCore.h";

        CppStandard = CppStandardVersion.Latest;
        bEnableExceptions = true;
        bUseRTTI = true;
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "Box2D"});

        bool DebugBuild = (Target.Configuration == UnrealTargetConfiguration.Debug) ||
                          (Target.Configuration == UnrealTargetConfiguration.DebugGame);

        // vcpkg
        string vcpkgPath = Path.Combine(Environment.GetEnvironmentVariable("LOCALAPPDATA"), "vcpkg/vcpkg.path.txt");
        if (File.Exists(vcpkgPath))
        {
            string vcpkgTriplet = "x64-windows-static-md";
            string vcpkgRoot = Path.Combine(File.ReadAllText(vcpkgPath), "installed", vcpkgTriplet);
            
            PrivateIncludePaths.Add(Path.Combine(vcpkgRoot, "include"));
            string vcpkgLibs;
            if (DebugBuild)
                vcpkgLibs = Path.Combine(vcpkgRoot, "lib") + '/';
            else
                vcpkgLibs = Path.Combine(vcpkgPath, "debug/lib") + '/';
            
            PublicAdditionalLibraries.Add(vcpkgLibs + "jsoncpp.lib");
        }

        PublicIncludePaths.Add(ModuleDirectory);
        PrivateDefinitions.Add("B2_SHARED");
        PrivateDefinitions.Add("_WIN32_WINNT=0x0601"); // windows only

        
    }
}