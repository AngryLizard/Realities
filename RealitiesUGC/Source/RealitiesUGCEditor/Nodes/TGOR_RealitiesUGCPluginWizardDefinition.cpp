// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_RealitiesUGCPluginWizardDefinition.h"
#include "Interfaces/IPluginManager.h"
#include "SlateBasics.h"

#define LOCTEXT_NAMESPACE "SimpleUGCPluginWizard"

FRealitiesUGCPluginWizardDefinition::FRealitiesUGCPluginWizardDefinition()
{
	PluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("RealitiesUGC"))->GetBaseDir();
	// Find the Content Only Template that ships with the plugin. 
	// Download the Robo Recall Mod Kit and check the Plugins/OdinEditor code for how to build and use your own UGC templates from your game content
	BackingTemplate = MakeShareable(new FPluginTemplateDescription(FText(), FText(), TEXT("BaseTemplate"), true, EHostType::Runtime));
	BackingTemplatePath = PluginBaseDir / TEXT("Templates") / BackingTemplate->OnDiskPath;
}

const TArray<TSharedRef<FPluginTemplateDescription>>& FRealitiesUGCPluginWizardDefinition::GetTemplatesSource() const
{
	return TemplateDefinitions;
}

void FRealitiesUGCPluginWizardDefinition::OnTemplateSelectionChanged(TArray<TSharedRef<FPluginTemplateDescription>> InSelectedItems, ESelectInfo::Type SelectInfo)
{
	SelectedTemplates = InSelectedItems;
}

TArray<TSharedPtr<FPluginTemplateDescription>> FRealitiesUGCPluginWizardDefinition::GetSelectedTemplates() const
{
	TArray<TSharedPtr<FPluginTemplateDescription>> SelectedTemplatePtrs;

	for (TSharedRef<FPluginTemplateDescription> Ref : SelectedTemplates)
	{
		SelectedTemplatePtrs.Add(Ref);
	}

	return SelectedTemplatePtrs;
}

void FRealitiesUGCPluginWizardDefinition::ClearTemplateSelection()
{
	SelectedTemplates.Empty();
}

bool FRealitiesUGCPluginWizardDefinition::HasValidTemplateSelection() const
{
	// A mod should be created even if no templates are actually selected
	return true;
}

bool FRealitiesUGCPluginWizardDefinition::CanContainContent() const 
{
	bool bHasContent = SelectedTemplates.Num() == 0;	// if no templates are selected, by default it is a content mod

	if (!bHasContent)
	{
		for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
		{
			// If at least one module can contain content, it's a content mod. Otherwise, it's a pure code mod.
			if (Template->bCanContainContent)
			{
				bHasContent = true;
				break;
			}
		}
	}

	return bHasContent;
}

bool FRealitiesUGCPluginWizardDefinition::HasModules() const
{
	bool bHasModules = false;

	for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
	{
		if (FPaths::DirectoryExists(PluginBaseDir / TEXT("Templates") / Template->OnDiskPath / TEXT("Source")))
		{
			bHasModules = true;
			break;
		}
	}

	return bHasModules;
}

bool FRealitiesUGCPluginWizardDefinition::IsMod() const
{
	return true;
}

void FRealitiesUGCPluginWizardDefinition::OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState)
{
}

ECheckBoxState FRealitiesUGCPluginWizardDefinition::GetShowOnStartupCheckBoxState() const
{
	return ECheckBoxState();
}

FText FRealitiesUGCPluginWizardDefinition::GetInstructions() const
{
	return LOCTEXT("CreateNewUGCPanel", "Give your new TGOR Mod package a name and Click 'Create Mod' to make a new content only TGOR Mod package.");
}

TSharedPtr<SWidget> FRealitiesUGCPluginWizardDefinition::GetCustomHeaderWidget()
{
	if ( !CustomHeaderWidget.IsValid() )
	{
		FString IconPath;
		GetPluginIconPath(IconPath);

		const FName BrushName(*IconPath);
		const FIntPoint Size = FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(BrushName);
		if ((Size.X > 0) && (Size.Y > 0))
		{
			IconBrush = MakeShareable(new FSlateDynamicImageBrush(BrushName, FVector2D(Size.X, Size.Y)));
		}

		CustomHeaderWidget = SNew(SHorizontalBox)
			// Header image
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.0f)
			[
				SNew(SBox)
				.WidthOverride(80.0f)
				.HeightOverride(80.0f)
				[
					SNew(SImage)
					.Image(IconBrush.IsValid() ? IconBrush.Get() : nullptr)
				]
			];
	}

	return CustomHeaderWidget;
}

bool FRealitiesUGCPluginWizardDefinition::GetPluginIconPath(FString& OutIconPath) const
{
	// Replace this file with your own 128x128 image if desired.
	OutIconPath = BackingTemplatePath / TEXT("Resources/Icon128.png");
	return false;
}

bool FRealitiesUGCPluginWizardDefinition::GetTemplateIconPath(TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const
{
	FString TemplateName = InTemplate->Name.ToString();

	OutIconPath = PluginBaseDir / TEXT("Resources");

	if (TemplateToIconMap.Contains(TemplateName))
	{
		OutIconPath /= TemplateToIconMap[TemplateName];
	}
	else
	{
		// Couldn't find a suitable icon to use for this template, so use the default one instead
		OutIconPath /= TEXT("Icon128.png");
	}
	
	return false;
}

FString FRealitiesUGCPluginWizardDefinition::GetPluginFolderPath() const
{
	return BackingTemplatePath;
}

EHostType::Type FRealitiesUGCPluginWizardDefinition::GetPluginModuleDescriptor() const
{
	return BackingTemplate->ModuleDescriptorType;
}

ELoadingPhase::Type FRealitiesUGCPluginWizardDefinition::GetPluginLoadingPhase() const
{
	return BackingTemplate->LoadingPhase;
}

TArray<FString> FRealitiesUGCPluginWizardDefinition::GetFoldersForSelection() const
{
	TArray<FString> SelectedFolders;
	SelectedFolders.Add(BackingTemplatePath);	// This will always be a part of the mod plugin

	for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
	{
		SelectedFolders.AddUnique(PluginBaseDir / TEXT("Templates") / Template->OnDiskPath);
	}

	return SelectedFolders;
}

void FRealitiesUGCPluginWizardDefinition::PluginCreated(const FString& PluginName, bool bWasSuccessful) const
{
	// Override Category to UGC
	if (bWasSuccessful)
	{
		TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName);
		if (Plugin != nullptr)
		{
			FPluginDescriptor Desc = Plugin->GetDescriptor();
			Desc.Category = "TGOR_Mod";
			FText UpdateFailureText;
			Plugin->UpdateDescriptor(Desc, UpdateFailureText);
		}
	}
}

#undef LOCTEXT_NAMESPACE
