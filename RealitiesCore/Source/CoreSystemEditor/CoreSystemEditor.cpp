// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreSystemEditor.h"
#include "PropertyEditorModule.h"

#include "CoreSystemExtension/Pins/TGORNode_PinFactory.h"

#define LOCTEXT_NAMESPACE "FCoreSystemEditorModule"

void FCoreSystemEditorModule::StartupModule()
{
	srand(time(NULL));

	// Make and register pin factory
	TSharedPtr<FGATGORNode_PanelGraphPinFactory> GAAttributePanelGraphPinFactory = MakeShareable(new FGATGORNode_PanelGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(GAAttributePanelGraphPinFactory);
}

void FCoreSystemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCoreSystemEditorModule, CoreSystemEditor)