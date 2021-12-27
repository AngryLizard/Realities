// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISequencerModule.h"
#include "SequencerCustomizationManager.h"
#include "Modules/ModuleInterface.h"

class IControlRigEditor;

class FRealitiesSequenceCustomization : public ISequencerCustomization
{
public:
	FRealitiesSequenceCustomization();

	virtual void RegisterSequencerCustomization(FSequencerCustomizationBuilder& Builder) override;
	virtual void UnregisterSequencerCustomization() override;

private:
	void ExtendSequencerToolbar(FToolBarBuilder& ToolbarBuilder);
	TSharedRef<class SWidget> MakeModifiersMenu();
	void BindCommands();

	ISequencer* Sequencer;
	TSharedRef<FUICommandList> ModifierCommandBindings;
};

/**
 * The public interface to this module
 */
class FRealitiesAnimationEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void UpdateSequencer(TSharedRef<ISequencer> Sequencer);

	TWeakPtr<ISequencer> GetSequencer() const { return CurrentSequencer; };

private:

	FDelegateHandle OnSequencerCreatedHandle;
	TWeakPtr<ISequencer> CurrentSequencer;
};