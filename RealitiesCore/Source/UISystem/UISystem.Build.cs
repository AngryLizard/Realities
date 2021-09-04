// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UISystem : ModuleRules
{
	public UISystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

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
				"Paper2D",
				"UMG",
				"InputCore"
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
