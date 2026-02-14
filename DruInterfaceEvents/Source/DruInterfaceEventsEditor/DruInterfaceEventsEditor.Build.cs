// Copyright Andrei Sudarikov. All Rights Reserved.

using UnrealBuildTool;

public class DruInterfaceEventsEditor : ModuleRules
{
    public DruInterfaceEventsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "BlueprintGraph",
            "CoreUObject",
            "DruInterfaceEvents",
            "Engine",
            "GraphEditor",
            "KismetCompiler",
            "Slate",
            "SlateCore",
            "UnrealEd",
        ]);
    }
}
