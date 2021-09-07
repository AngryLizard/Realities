// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MovementContent : ModuleRules
{
	public MovementContent(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"AttributeSystem",
				"DimensionSystem",
				"AnimationSystem",
				"CustomisationSystem",
				"SocketSystem",
				"PhysicsSystem",
				"MovementSystem",

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
