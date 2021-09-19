// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimationSystemEditor.h"
#include "PropertyEditorModule.h"
#include "TGORNode_AnimationInstanceVariableSet.h"

#define LOCTEXT_NAMESPACE "FAnimationSystemEditorModule"

void FAnimationSystemEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("TGOR_AnimationVariableSet", FOnGetDetailCustomizationInstance::CreateStatic(&FAnimationInstanceVariableSetDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("TGOR_ArchetypeVariableSet", FOnGetDetailCustomizationInstance::CreateStatic(&FAnimationInstanceVariableSetDetails::MakeInstance));
}

void FAnimationSystemEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAnimationSystemEditorModule, AnimationSystemEditor)