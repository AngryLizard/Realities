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

				"CoreSystem",
				"UISystem",

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
				"UMG",
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
