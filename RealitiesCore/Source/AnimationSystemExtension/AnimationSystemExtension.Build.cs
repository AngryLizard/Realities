// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimationSystemExtension : ModuleRules
{
	public AnimationSystemExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"AnimationSystem",

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
				"BlueprintGraph",
				"KismetCompiler",
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
