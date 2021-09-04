// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DimensionSystem : ModuleRules
{
	public DimensionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",
				"RealitiesRendering",

				"CoreSystem",

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
				"ReplicationGraph",
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
