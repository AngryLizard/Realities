// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlayerSystem : ModuleRules
{
	public PlayerSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"UISystem",
				"DimensionSystem",
				"CreatureSystem",
				"KnowledgeSystem",
				"SimulationSystem",
				"CreatureSystem",
				"TargetSystem",
				"ActionSystem",
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
				"UMG"
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
