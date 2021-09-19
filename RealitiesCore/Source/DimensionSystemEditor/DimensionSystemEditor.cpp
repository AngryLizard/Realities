// Copyright Epic Games, Inc. All Rights Reserved.

#include "DimensionSystemEditor.h"
#include "PropertyEditorModule.h"

#include "DimensionSystem/Gameplay/TGOR_DimensionWorldSettings.h"
#include "DimensionSystemEditor/TGORNode_ConnectionSelection.h"
#include "DimensionSystemEditor/TGORNode_DimensionDetails.h"
#include "DimensionSystem/TGOR_DimensionInstance.h"

#define LOCTEXT_NAMESPACE "FDimensionSystemEditorModule"

void FDimensionSystemEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_ConnectionSelection", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_ConnectionSelection::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(ATGOR_DimensionWorldSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FTGORNode_DimensionDetails::MakeInstance));


}

void FDimensionSystemEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDimensionSystemEditorModule, DimensionSystemEditor)