// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RealitiesAnimationDeveloper : ModuleRules
{
	public RealitiesAnimationDeveloper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Slate",
                    "SlateCore",
                    "Engine"
				}
				);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AnimGraphRuntime",
				"RealitiesUtility"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
