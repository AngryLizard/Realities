// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Customisation : ModuleRules
{
	public Customisation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"CustomisationSystem",

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
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
