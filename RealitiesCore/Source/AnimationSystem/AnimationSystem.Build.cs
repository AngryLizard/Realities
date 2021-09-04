// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimationSystem : ModuleRules
{
	public AnimationSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"DimensionSystem",
				"CustomisationSystem",
				"PhysicsSystem",
				"InventorySystem",

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
				"ControlRig",
				"AnimGraphRuntime",
			}
			);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd"
				}
				);
		}

		PublicDefinitions.Add("USE_RTTI");
	}
}
