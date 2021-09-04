// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RealitiesProceduralEditor : ModuleRules
{
	public RealitiesProceduralEditor(ReadOnlyTargetRules Target) : base(Target)
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
                    "Engine",
				}
				);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"PropertyEditor",
				"EditorWidgets",
				"RenderCore",
				"RHI",
				"ProceduralMeshComponent",
				"MeshDescription",
				"StaticMeshDescription",
				"EditorScriptingUtilities",
				"AssetTools",
				"AssetRegistry",
				"RealitiesUtility",
				"RealitiesProcedural",
				"TextureEditor",
				"InputCore",
				"PlacementMode"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
