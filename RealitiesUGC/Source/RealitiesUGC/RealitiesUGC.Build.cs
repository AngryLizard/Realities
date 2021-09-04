// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RealitiesUGC : ModuleRules
{
	public RealitiesUGC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] {
			// ... add public include paths required here ...
		});
				
		
		PrivateIncludePaths.AddRange(new string[] {
			// ... add other private include paths required here ...
		});
			
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"AssetRegistry",
				
			// ... add other public dependencies that you statically link with here ...
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"CoreUObject",
			"Engine",
            "Projects",
			"UMG",
			"Slate",
			"SlateCore",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"RealitiesUtility"
			// ... add private dependencies that you statically link with here ...	
		});


		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"UnrealEd"
			});
		}


		DynamicallyLoadedModuleNames.AddRange(new string[] {
			// ... add any modules that your module loads dynamically here ...
		});

		PublicDefinitions.Add("USE_RTTI");
	}
}
