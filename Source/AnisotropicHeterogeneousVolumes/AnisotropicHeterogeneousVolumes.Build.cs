using UnrealBuildTool;

public class AnisotropicHeterogeneousVolumes : ModuleRules
{
    public AnisotropicHeterogeneousVolumes(ReadOnlyTargetRules Target) : base(Target)
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
