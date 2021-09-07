// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CoreContent : ModuleRules
{
	public CoreContent(ReadOnlyTargetRules Target) : base(Target)
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
