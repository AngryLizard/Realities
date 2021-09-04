// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CreatureSystem : ModuleRules
{
	public CreatureSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"DimensionSystem",
				"AttributeSystem",
				"InventorySystem",
				"AnimationSystem",
				"SimulationSystem",
				"MovementSystem",
				"PhysicsSystem",
				"TargetSystem",
				"ActionSystem",
				"KnowledgeSystem",
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
