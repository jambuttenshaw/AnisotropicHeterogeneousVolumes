using UnrealBuildTool;

public class AnisotropicVolumetrics : ModuleRules
{
    public AnisotropicVolumetrics(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "RenderCore",
            "Renderer",
            "RHI",
            "Shaders"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            
        });
        
    }
}
