// Copyright Andrei Sudarikov. All Rights Reserved.

using UnrealBuildTool;

public class DruInterfaceEvents : ModuleRules
{
    public DruInterfaceEvents(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "CoreUObject",
            "Engine",
        ]);
    }
}
