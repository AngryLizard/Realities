// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RealitiesAnimationEditor : ModuleRules
{
	public RealitiesAnimationEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Slate",
                    "SlateCore",
                    "Engine"
				}
				);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUtility",
				"ControlRig",
				"ControlRigEditor",
				"Sequencer",
				"LevelSequence",
				"MovieScene",
				"MovieSceneTracks",
				"LevelSequenceEditor",
				"EditorStyle",
				"UnrealEd"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
