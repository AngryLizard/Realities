// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomisationSystemEditor.h"
#include "PropertyEditorModule.h"
#include "TGORNode_GeometryDetails.h"

#define LOCTEXT_NAMESPACE "FCustomisationSystemEditorModule"

void FCustomisationSystemEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(USkeletalMesh::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FTGORNode_GeometryDetails::MakeInstance));
}

void FCustomisationSystemEditorModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomisationSystemEditorModule, CustomisationSystemEditor)