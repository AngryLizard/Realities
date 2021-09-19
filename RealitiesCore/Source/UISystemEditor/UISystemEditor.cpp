// Copyright Epic Games, Inc. All Rights Reserved.

#include "UISystemEditor.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FUISystemEditorModule"

void FUISystemEditorModule::StartupModule()
{
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_MenuVarParam", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_MenuParamCustomization::MakeInstance));
}

void FUISystemEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUISystemEditorModule, UISystemEditor)