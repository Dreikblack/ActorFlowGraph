using UnrealBuildTool;

public class ActorFlowGraphEditor : ModuleRules
{
    public ActorFlowGraphEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "GraphEditor",
            "InputCore",
            "ActorFlowGraph",
            "BlueprintGraph",
            "ToolMenus",
            "KismetWidgets",
            "Projects",
            "EditorFramework"
        });
    }
}
