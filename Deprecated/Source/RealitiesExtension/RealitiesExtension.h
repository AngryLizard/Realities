// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

class FRealitiesExtension : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
	
	/** Populates a box with all mods */
	void PopulateModCollection(TSharedRef<SVerticalBox> Box);

	/** Removes and rebuilds second slot */
	FReply RefreshMods(TSharedRef<SVerticalBox> Box);

	/** Called when clicked the button */
	void OpenModLoaderWindow();

private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<class FUICommandList> PluginCommands;
};