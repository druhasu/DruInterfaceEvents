// Copyright Andrei Sudarikov. All Rights Reserved.

using UnrealBuildTool;

public class DruInterfaceEventsTests : ModuleRules
{
    public DruInterfaceEventsTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "CoreUObject",
            "DruInterfaceEvents",
            "Engine",
        ]);
    }
}
