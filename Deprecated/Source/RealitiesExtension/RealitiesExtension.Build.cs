// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RealitiesExtension : ModuleRules
{
	public RealitiesExtension(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
           "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore",
            "BlueprintGraph",
            "AnimGraphRuntime",
            "Blutility",
            "AnimGraph"
            }
        );
        
        PrivateDependencyModuleNames.AddRange(new string[] {
           // ... add private dependencies that you statically link with here ...
           "Core",
           "CoreUObject",
           "InputCore",
           "Engine",
           "AssetTools",
           "UnrealEd", // for FAssetEditorManager
           "BlueprintGraph",
           "GraphEditor",
           "Kismet",  // for FWorkflowCentricApplication
           "KismetCompiler",
           "KismetWidgets",
           "PropertyEditor",
           "EditorStyle",
           "Slate",
           "SlateCore",
           "Sequencer",
           "DetailCustomizations",
           "Settings",
           "RenderCore",
           "AnimGraph",

           "Realities",
           "Projects",
           "LevelEditor",
           "EditorWidgets",
            "AnimGraphRuntime"
            }
        );
    }
}
