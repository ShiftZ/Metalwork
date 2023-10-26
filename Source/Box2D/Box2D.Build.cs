using UnrealBuildTool;

public class Box2D : ModuleRules
{
    public Box2D(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.NoPCHs;

        PublicDefinitions.Add("B2_USER_SETTINGS");
        if (Target.Type == TargetType.Editor) PublicDefinitions.Add("B2_SHARED");

        PublicIncludePaths.Add(ModuleDirectory);

        PrivateDefinitions.Add("box2d_EXPORTS");
        PrivateDefinitions.Add("_CRT_SECURE_NO_DEPRECATE"); // msvc only
    }
}