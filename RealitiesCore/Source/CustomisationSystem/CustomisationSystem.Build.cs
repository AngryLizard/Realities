// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CustomisationSystem : ModuleRules
{
	public CustomisationSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RealitiesUGC",
				"RealitiesUtility",

				"CoreSystem",
				"DimensionSystem",
				"KnowledgeSystem",
				"TargetSystem",
				"UISystem",
				"AttributeSystem",

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
				"AnimationCore",
				"RenderCore",
				"Json",
				"UMG",
				"RHI",
			}
			);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[] 
				{
					"UnrealEd",
					"DesktopPlatform",
				}
				);
		}

		PublicDefinitions.Add("USE_RTTI");
	}
}
