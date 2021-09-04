// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionSystem : ModuleRules
{
	public ActionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"UISystem",
				"AttributeSystem",
				"DimensionSystem",
				"TargetSystem",
				"InventorySystem",
				"AnimationSystem",
				"SocketSystem",
				"PhysicsSystem",
				"MovementSystem",
				"SimulationSystem",

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
