// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "RealitiesExtension.h"
#include <ctime>

/////////////////////////////////////////

#include "Windows/TGOR_WindowStyle.h"
#include "Windows/TGOR_WindowCommands.h"
//#include "Windows/TGOR_ModListWidget.h"
#include "Nodes/Pins/TGORNode_PinFactory.h"
//#include "Nodes/DetailsPanes/TGORNode_MenuParamCustomization.h"
#include "Nodes/DetailsPanes/TGORNode_MenuContentTypes.h"
#include "Nodes/DetailsPanes/TGORNode_PortalSelection.h"
#include "Nodes/TGOR_AnimationVariableSet.h"

/////////////////////////////////////////

#include "Realities/Mod/TGOR_Mod.h"

/////////////////////////////////////////

#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
//#include "Nodes/Pins/TGORNode_MenuVarParamPin.h"


static const FName RealitiesTabName("Realities");

#define LOCTEXT_NAMESPACE "RealitiesExtension"

void FRealitiesExtension::StartupModule()
{
	srand(time(NULL));

	CTGOR_WindowStyle::Initialize();
	CTGOR_WindowStyle::ReloadTextures();

	CTGOR_WindowCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		CTGOR_WindowCommands::Get().OpenExtensionWindow,
		FExecuteAction::CreateRaw(this, &FRealitiesExtension::OpenModLoaderWindow),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FRealitiesExtension::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	/*
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FRealitiesExtension::AddToolbarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	*/

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(RealitiesTabName, FOnSpawnTab::CreateRaw(this, &FRealitiesExtension::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("RealitiesTabTitle", "Realities"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	

	// Make and register pin factory
	TSharedPtr<FGATGORNode_PanelGraphPinFactory> GAAttributePanelGraphPinFactory = MakeShareable(new FGATGORNode_PanelGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(GAAttributePanelGraphPinFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom properties
	PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_Mod", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_MenuContentTypes::MakeInstance));
	//PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_MenuVarParam", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_MenuParamCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("TGOR_PortalSelection", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTGORNode_PortalSelection::MakeInstance));

	PropertyModule.RegisterCustomClassLayout("TGOR_AnimationVariableSet", FOnGetDetailCustomizationInstance::CreateStatic(&FAnimationVariableSetDetails::MakeInstance));
}

void FRealitiesExtension::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	CTGOR_WindowStyle::Shutdown();

	CTGOR_WindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(RealitiesTabName);

}


void FRealitiesExtension::OpenModLoaderWindow()
{
	FGlobalTabmanager::Get()->InvokeTab(RealitiesTabName);
}


void FRealitiesExtension::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(CTGOR_WindowCommands::Get().OpenExtensionWindow);
}

void FRealitiesExtension::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(CTGOR_WindowCommands::Get().OpenExtensionWindow);
}



TSharedRef<class SDockTab> FRealitiesExtension::OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs)
{

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);
//		[
			//SNew(STGOR_ModListWidget)
//		];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRealitiesExtension, RealitiesExtension);
