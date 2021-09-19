// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimationSystemEditor : ModuleRules
{
	public AnimationSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"AnimationSystem",
				"AnimationSystemExtension",

				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"PropertyEditor",
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
