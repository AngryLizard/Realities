// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DimensionSystemEditor : ModuleRules
{
	public DimensionSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"TargetSystem",
				"DimensionSystem",
				"CoreSystemEditor",

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
				"PropertyEditor",
				"UnrealEd",
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
