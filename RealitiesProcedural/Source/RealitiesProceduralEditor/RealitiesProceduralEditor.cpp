// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitiesProceduralEditor.h"

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Nodes/RealitiesProceduralDetails.h"
#include "Nodes/TGORNode_NormalCurve.h"
#include "Nodes/TGORNode_UnitCurve.h"

#include "RealitiesProcedural/Actors/TGOR_ProceduralActor.h"
#include "RealitiesProcedural/Textures/TGOR_PaintGradient.h"
#include "RealitiesProceduralEditor/Factories/PaintGradient/TGOR_AssetTypeActions_PaintGradient.h"
#include "RealitiesProceduralEditor/Factories/PaintGradient/TGOR_PaintGradientThumbnailRenderer.h"
#include "RealitiesProcedural/Textures/TGOR_ExteriorGradient.h"
#include "RealitiesProceduralEditor/Factories/ExteriorGradient/TGOR_AssetTypeActions_ExteriorGradient.h"
#include "RealitiesProceduralEditor/Factories/ExteriorGradient/TGOR_ExteriorGradientThumbnailRenderer.h"
/*
#include "RealitiesProcedural/Textures/TGOR_WorldPainterTexture.h"
#include "RealitiesProceduralEditor/Factories/WorldPainter/TGOR_AssetTypeActions_WorldPainterTexture.h"
#include "RealitiesProceduralEditor/Factories/WorldPainter/TGOR_WorldPainterTextureThumbnailRenderer.h"
*/

#include "IPlacementModeModule.h"

#include "RealitiesProcedural/Actors/TGOR_WorldPainterCanvas.h"
#include "RealitiesProcedural/Actors/TGOR_WorldPainterLayer.h"
#include "RealitiesProcedural/Actors/TGOR_WorldPainterBrush.h"

#define LOCTEXT_NAMESPACE "FRealitiesProceduralEditor"

void FRealitiesProceduralEditor::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(ATGOR_ProceduralActor::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FRealitiesProceduralDetails::MakeInstance));
	}

	FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
	IAssetTools& AssetTools = AssetToolsModule.Get();

	AssetTypeActions_PaintGradient = MakeShareable(new FTGOR_AssetTypeActions_PaintGradient);
	AssetTools.RegisterAssetTypeActions(AssetTypeActions_PaintGradient->AsShared());
	UThumbnailManager::Get().RegisterCustomRenderer(UTGOR_PaintGradient::StaticClass(), UTGOR_PaintGradientThumbnailRenderer::StaticClass());

	AssetTypeActions_ExteriorGradient = MakeShareable(new FTGOR_AssetTypeActions_ExteriorGradient);
	AssetTools.RegisterAssetTypeActions(AssetTypeActions_ExteriorGradient->AsShared());
	UThumbnailManager::Get().RegisterCustomRenderer(UTGOR_ExteriorGradient::StaticClass(), UTGOR_ExteriorGradientThumbnailRenderer::StaticClass());

	/*
	AssetTypeActions_WorldPainterTexture = MakeShareable(new FTGOR_AssetTypeActions_WorldPainterTexture);
	AssetTools.RegisterAssetTypeActions(AssetTypeActions_WorldPainterTexture->AsShared());
	UThumbnailManager::Get().RegisterCustomRenderer(UTGOR_WorldPainterTexture::StaticClass(), UTGOR_WorldPainterTextureThumbnailRenderer::StaticClass());
	*/

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_NormalCurve", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_NormalCurve::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_UnitCurve", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_UnitCurve::MakeInstance));

	IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
	PlacementModeModule.RegisterPlacementCategory(FPlacementCategoryInfo(LOCTEXT("WorldPainter_CategoryName", "World Painter"), "WorldPainter", TEXT("PMWorldPainter"), 69));
	PlacementModeModule.OnPlacementModeCategoryRefreshed().AddRaw(this, &FRealitiesProceduralEditor::OnPlacementModeRefresh);
}

void FRealitiesProceduralEditor::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (UObjectInitialized())
	{
		FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
		IAssetTools& AssetTools = AssetToolsModule.Get();
		AssetTools.UnregisterAssetTypeActions(AssetTypeActions_PaintGradient->AsShared());
		AssetTools.UnregisterAssetTypeActions(AssetTypeActions_ExteriorGradient->AsShared());
		//AssetTools.UnregisterAssetTypeActions(AssetTypeActions_WorldPainterTexture->AsShared());
	}
}

void FRealitiesProceduralEditor::OnPlacementModeRefresh(FName CategoryName)
{
	static FName VolumeName = FName(TEXT("WorldPainter"));
	if (CategoryName == VolumeName)
	{
		IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
		PlacementModeModule.RegisterPlaceableItem(CategoryName, MakeShareable(new FPlaceableItem(nullptr, FAssetData(ATGOR_WorldPainterCanvas::StaticClass()))));
		PlacementModeModule.RegisterPlaceableItem(CategoryName, MakeShareable(new FPlaceableItem(nullptr, FAssetData(ATGOR_WorldPainterLayer::StaticClass()))));
		PlacementModeModule.RegisterPlaceableItem(CategoryName, MakeShareable(new FPlaceableItem(nullptr, FAssetData(ATGOR_WorldPainterBrush::StaticClass()))));
	}
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRealitiesProceduralEditor, RealitiesProceduralEditor)
