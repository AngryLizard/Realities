// Copyright Epic Games, Inc. All Rights Reserved.

//#include "SimpleUGCEditorPrivatePCH.h"
#include "TGOR_RealitiesUGCCreator.h"

#include "TGOR_RealitiesUGCPluginWizardDefinition.h"
#include "Widgets/Docking/SDockTab.h"

// This depends on the Plugin Browser module to work correctly...
#include "IPluginBrowser.h"



#define LOCTEXT_NAMESPACE "FTGOR_RealitiesUGCCreator"

const FName FTGOR_RealitiesUGCCreator::SimpleUGCEditorPluginCreatorName("SimpleUGCPluginCreator");

FTGOR_RealitiesUGCCreator::FTGOR_RealitiesUGCCreator()
{
	RegisterTabSpawner();
}

FTGOR_RealitiesUGCCreator::~FTGOR_RealitiesUGCCreator()
{
	UnregisterTabSpawner();
}

void FTGOR_RealitiesUGCCreator::OpenNewPluginWizard(bool bSuppressErrors) const
{
	if (IPluginBrowser::IsAvailable())
	{
		FGlobalTabmanager::Get()->TryInvokeTab(SimpleUGCEditorPluginCreatorName);
	}
	else if (!bSuppressErrors)
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			LOCTEXT("PluginBrowserDisabled", "Creating a game mod requires the use of the Plugin Browser, but it is currently disabled."));
	}
}

void FTGOR_RealitiesUGCCreator::RegisterTabSpawner()
{
	FTabSpawnerEntry& Spawner = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SimpleUGCEditorPluginCreatorName,
																FOnSpawnTab::CreateRaw(this, &FTGOR_RealitiesUGCCreator::HandleSpawnPluginTab));

	// Set a default size for this tab
	FVector2D DefaultSize(800.0f, 500.0f);
	FTabManager::RegisterDefaultTabWindowSize(SimpleUGCEditorPluginCreatorName, DefaultSize);

	Spawner.SetDisplayName(LOCTEXT("NewUGCTabHeader", "Create New UGC Package"));
	Spawner.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTGOR_RealitiesUGCCreator::UnregisterTabSpawner()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SimpleUGCEditorPluginCreatorName);
}

TSharedRef<SDockTab> FTGOR_RealitiesUGCCreator::HandleSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	check(IPluginBrowser::IsAvailable());
	return IPluginBrowser::Get().SpawnPluginCreatorTab(SpawnTabArgs, MakeShared<FRealitiesUGCPluginWizardDefinition>());
}

#undef LOCTEXT_NAMESPACE