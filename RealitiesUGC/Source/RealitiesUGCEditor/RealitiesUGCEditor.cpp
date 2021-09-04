// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitiesUGCEditor/RealitiesUGCEditor.h"
#include "Nodes/TGOR_RealitiesUGCEditorStyle.h"
#include "Nodes/TGOR_RealitiesUGCEditorCommands.h"
#include "Nodes/TGOR_RealitiesUGCCreator.h"
#include "Nodes/TGOR_RealitiesUGCPackager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "LevelEditor.h"

static const FName SimpleUGCEditorTabName("SimpleUGCEditor");

#define LOCTEXT_NAMESPACE "FRealitiesUGCEditor"

void FRealitiesUGCEditor::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UGCCreator = MakeShared<FTGOR_RealitiesUGCCreator>();
	UGCPackager = MakeShared<FTGOR_RealitiesUGCPackager>();

	FTGOR_RealitiesUGCEditorStyle::Initialize();
	FTGOR_RealitiesUGCEditorStyle::ReloadTextures();

	FTGOR_RealitiesUGCEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTGOR_RealitiesUGCEditorCommands::Get().CreateUGCAction,
		FExecuteAction::CreateRaw(this, &FRealitiesUGCEditor::CreateUGCButtonClicked),
		FCanExecuteAction()
	);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	// Add commands
	{
		FName MenuSection = "FileProject";
		FName ToolbarSection = "Misc";

		// Add creator button to the menu
		{
			TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
			MenuExtender->AddMenuExtension(MenuSection, EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FRealitiesUGCEditor::AddUGCCreatorMenuExtension));

			LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
		}

		// Add creator button to the toolbar
		{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
			ToolbarExtender->AddToolBarExtension(ToolbarSection, EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FRealitiesUGCEditor::AddUGCCreatorToolbarExtension));

			LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		}

		// Add packager button to the menu
		{
			TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
			MenuExtender->AddMenuExtension(MenuSection, EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FRealitiesUGCEditor::AddUGCPackagerMenuExtension));

			LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
		}

		// Add packager button to the toolbar
		{
			TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
			ToolbarExtender->AddToolBarExtension(ToolbarSection, EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FRealitiesUGCEditor::AddUGCPackagerToolbarExtension));

			LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		}
	}
}

void FRealitiesUGCEditor::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FTGOR_RealitiesUGCEditorStyle::Shutdown();

	FTGOR_RealitiesUGCEditorCommands::Unregister();

}

void FRealitiesUGCEditor::CreateUGCButtonClicked()
{
	if (UGCCreator.IsValid())
	{
		UGCCreator->OpenNewPluginWizard();
	}
}

void FRealitiesUGCEditor::AddUGCCreatorMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FTGOR_RealitiesUGCEditorCommands::Get().CreateUGCAction);
}

void FRealitiesUGCEditor::AddUGCCreatorToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FTGOR_RealitiesUGCEditorCommands::Get().CreateUGCAction);
}

void FRealitiesUGCEditor::AddUGCPackagerMenuExtension(FMenuBuilder& Builder)
{
	FTGOR_RealitiesUGCPackager* Packager = UGCPackager.Get();

	Builder.AddSubMenu(LOCTEXT("PackageUGCMenu_Label", "Package UGC"),
		LOCTEXT("PackageUGCMenu_Tooltip", "Share and distribute UGC"),
		FNewMenuDelegate::CreateRaw(Packager, &FTGOR_RealitiesUGCPackager::GeneratePackagerMenuContent),
		false,
		FSlateIcon(FTGOR_RealitiesUGCEditorStyle::GetStyleSetName(), "SimpleUGCEditor.PackageUGCAction")
	);
}

void FRealitiesUGCEditor::AddUGCPackagerToolbarExtension(FToolBarBuilder& Builder)
{
	FTGOR_RealitiesUGCPackager* Packager = UGCPackager.Get();

	Builder.AddComboButton(FUIAction(),
		FOnGetContent::CreateSP(Packager, &FTGOR_RealitiesUGCPackager::GeneratePackagerComboButtonContent),
		LOCTEXT("PackageUGC_Label", "Package UGC"),
		LOCTEXT("PackageUGC_Tooltip", "Share and distribute UGC"),
		FSlateIcon(FTGOR_RealitiesUGCEditorStyle::GetStyleSetName(), "SimpleUGCEditor.PackageUGCAction")
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRealitiesUGCEditor, RealitiesUGCEditor)
