// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActorFlowGraph : ModuleRules
{
    public ActorFlowGraph(ReadOnlyTargetRules Target) : base(Target)
    {
        if (CppStandard is null || CppStandard != CppStandardVersion.Cpp20)
        {
            CppStandard = CppStandardVersion.Cpp20;
        }

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
        "Core",
        "CoreUObject",
        "Engine",
        "ApplicationCore"
        });
    }
}
