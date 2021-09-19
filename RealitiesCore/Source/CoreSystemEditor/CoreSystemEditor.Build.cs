// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CoreSystemEditor : ModuleRules
{
	public CoreSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"CoreSystemExtension",

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
				"BlueprintGraph",
				"GraphEditor",
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
