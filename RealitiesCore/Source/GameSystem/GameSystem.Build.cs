// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameSystem : ModuleRules
{
	public GameSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"DimensionSystem",
				"UISystem",
				"PlayerSystem",

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
				"UMG"
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
