// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_RealitiesUGCEditorCommands.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FTGOR_RealitiesUGCEditorCommands"

void FTGOR_RealitiesUGCEditorCommands::RegisterCommands()
{
	UI_COMMAND(CreateUGCAction, "Create UGC", "Create a new UGC package in a mod plugin", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(PackageUGCAction, "Package UGC", "Share and distribute your UGC", EUserInterfaceActionType::Button, FInputGesture());
}

TArray<TSharedPtr<FUICommandInfo>> FTGOR_RealitiesUGCEditorCommands::RegisterUGCCommands(const TArray<TSharedRef<class IPlugin>>& UGCList) const
{
	TArray<TSharedPtr<FUICommandInfo>> AvailableUGCActions;
	AvailableUGCActions.Reserve(UGCList.Num());

	FTGOR_RealitiesUGCEditorCommands* MutableThis = const_cast<FTGOR_RealitiesUGCEditorCommands*>(this);

	for (int32 Index = 0; Index < UGCList.Num(); ++Index)
	{
		AvailableUGCActions.Add(TSharedPtr<FUICommandInfo>());
		TSharedRef<IPlugin> UGC = UGCList[Index];

		FString CommandName = "UGCEditorUGC_" + UGC->GetName();

		FUICommandInfo::MakeCommandInfo(MutableThis->AsShared(),
			AvailableUGCActions[Index],
			FName(*CommandName),
			FText::FromString(UGC->GetName()),
			FText::FromString(UGC->GetBaseDir()),
			FSlateIcon(),
			EUserInterfaceActionType::Button,
			FInputGesture());
	}

	return AvailableUGCActions;
}

void FTGOR_RealitiesUGCEditorCommands::UnregisterUGCCommands(TArray<TSharedPtr<FUICommandInfo>>& UICommands) const
{
	FTGOR_RealitiesUGCEditorCommands* MutableThis = const_cast<FTGOR_RealitiesUGCEditorCommands*>(this);

	for (TSharedPtr<FUICommandInfo> Command : UICommands)
	{
		FUICommandInfo::UnregisterCommandInfo(MutableThis->AsShared(), Command.ToSharedRef());
	}
}

#undef LOCTEXT_NAMESPACE