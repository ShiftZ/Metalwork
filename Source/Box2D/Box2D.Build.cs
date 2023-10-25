using UnrealBuildTool;

public class Box2D : ModuleRules
{
    public Box2D(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.NoPCHs;
        
        PublicDefinitions.AddRange(new string[] {"B2_USER_SETTINGS", "B2_SHARED"});
        PublicIncludePaths.Add(ModuleDirectory);

        PrivateDefinitions.Add("box2d_EXPORTS");
        PrivateDefinitions.Add("_CRT_SECURE_NO_DEPRECATE"); // msvc only
    }
}