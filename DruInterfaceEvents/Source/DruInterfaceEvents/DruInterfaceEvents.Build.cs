// Copyright Andrei Sudarikov. All Rights Reserved.

using System.Collections.Generic;
using UnrealBuildTool;

public class DruInterfaceEvents : ModuleRules
{
    public static List<string> SetupModules { get; set; } = [];

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

    public static void Setup(ModuleRules module)
    {
        string thisModuleName = nameof(DruInterfaceEvents);

        if (!module.PrivateDependencyModuleNames.Contains(thisModuleName) &&
            !module.PublicDependencyModuleNames.Contains(thisModuleName))
        {
            // add to Public dependencies by default for ease of use
            module.PublicDependencyModuleNames.Add(thisModuleName);
        }

        SetupModules.Add(module.Name);

#if UE_5_8_OR_LATER
        module.FilesToGenerate.Add(
            $"{module.Name}.IEvents.gen.cpp",
            [
                $"#if __has_include(\"{module.Name}.IEvents.gen.keep\")",
                $"#include \"{module.Name}.IEvents.gen.keep\"",
                "#endif",
            ]);
#endif
    }
}
