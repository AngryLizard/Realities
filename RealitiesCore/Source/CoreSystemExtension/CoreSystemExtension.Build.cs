// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CoreSystemExtension : ModuleRules
{
	public CoreSystemExtension(ReadOnlyTargetRules Target) : base(Target)
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
				"PropertyEditor",
				"BlueprintGraph",
				"KismetCompiler",
				"GraphEditor",
				"UnrealEd"
			}
			);
		
		PublicDefinitions.Add("USE_RTTI");
	}
}
