// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitiesAnimationEditor.h"

#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Modifiers/TGOR_ModifierCommands.h"
#include "Modifiers/TGOR_ModifierMirror.h"
#include "IControlRigEditorModule.h"
#include "LevelSequence.h"

#define LOCTEXT_NAMESPACE "FRealitiesAnimationEditor"

FRealitiesSequenceCustomization::FRealitiesSequenceCustomization()
: ModifierCommandBindings(new FUICommandList())
{
}

void FRealitiesSequenceCustomization::RegisterSequencerCustomization(FSequencerCustomizationBuilder& Builder)
{
	Sequencer = &Builder.GetSequencer();

	FSequencerCustomizationInfo Customization;

	TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension("Base Commands", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw(this, &FRealitiesSequenceCustomization::ExtendSequencerToolbar));
	Customization.ToolbarExtender = ToolbarExtender;

	Customization.OnAssetsDrop.BindLambda([](const TArray<UObject*>&, const FAssetDragDropOp&) -> ESequencerDropResult { return ESequencerDropResult::DropDenied; });
	Customization.OnClassesDrop.BindLambda([](const TArray<TWeakObjectPtr<UClass>>&, const FClassDragDropOp&) -> ESequencerDropResult { return ESequencerDropResult::DropDenied; });
	Customization.OnActorsDrop.BindLambda([](const TArray<TWeakObjectPtr<AActor>>&, const FActorDragDropOp&) -> ESequencerDropResult { return ESequencerDropResult::DropDenied; });

	Builder.AddCustomization(Customization);
}

void FRealitiesSequenceCustomization::UnregisterSequencerCustomization()
{
}

void FRealitiesSequenceCustomization::ExtendSequencerToolbar(FToolBarBuilder& ToolbarBuilder)
{
	BindCommands();

	ToolbarBuilder.AddSeparator();

	ToolbarBuilder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FRealitiesSequenceCustomization::MakeModifiersMenu),
		LOCTEXT("Modifiers", "Modifiers"),
		LOCTEXT("ModifiersToolTip", "Modifiers"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Actions")
	);
}

TSharedRef<SWidget> FRealitiesSequenceCustomization::MakeModifiersMenu()
{
	FRealitiesAnimationEditor& AnimationEditor = FModuleManager::LoadModuleChecked<FRealitiesAnimationEditor>("RealitiesAnimationEditor");
	FMenuBuilder MenuBuilder(true, ModifierCommandBindings);

	// selection range actions
	MenuBuilder.BeginSection("Track", LOCTEXT("TrackHeader", "Track Modidifiers"));
	{
		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().MirrorX);
		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().Loop);
		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().TimeOffsetHalf);

		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().MirrorLeftToRight);
		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().FlipLeftToRight);

		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().MirrorRightToLeft);
		MenuBuilder.AddMenuEntry(FTGOR_ModifierCommands::Get().FlipRightToLeft);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void FRealitiesSequenceCustomization::BindCommands()
{
	const FTGOR_ModifierCommands& Commands = FTGOR_ModifierCommands::Get();

	ModifierCommandBindings->MapAction(Commands.MirrorX, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().Mirror(); }));
	ModifierCommandBindings->MapAction(Commands.Loop, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().Loop(); }));
	ModifierCommandBindings->MapAction(Commands.TimeOffsetHalf, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().TimeOffsetHalf(); }));

	ModifierCommandBindings->MapAction(Commands.MirrorLeftToRight, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().FlipLeftToRight(false); }));
	ModifierCommandBindings->MapAction(Commands.FlipLeftToRight, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().FlipLeftToRight(true); }));

	ModifierCommandBindings->MapAction(Commands.MirrorRightToLeft, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().FlipRightToLeft(false); }));
	ModifierCommandBindings->MapAction(Commands.FlipRightToLeft, FExecuteAction::CreateLambda([]() { FTGOR_ModifierMirror().FlipRightToLeft(true); }));
}

void FRealitiesAnimationEditor::StartupModule()
{
	FTGOR_ModifierCommands::Register();

	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.GetSequencerCustomizationManager()->RegisterInstancedSequencerCustomization(ULevelSequence::StaticClass(),
		FOnGetSequencerCustomizationInstance::CreateLambda([]()
	{
		return new FRealitiesSequenceCustomization();
	}));

	OnSequencerCreatedHandle = SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateRaw(this, &FRealitiesAnimationEditor::UpdateSequencer));
}

void FRealitiesAnimationEditor::ShutdownModule()
{
	FTGOR_ModifierCommands::Unregister();

	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.UnregisterOnSequencerCreated(OnSequencerCreatedHandle);
}

void FRealitiesAnimationEditor::UpdateSequencer(TSharedRef<ISequencer> Sequencer)
{
	CurrentSequencer = Sequencer;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRealitiesAnimationEditor, RealitiesAnimationEditor)
