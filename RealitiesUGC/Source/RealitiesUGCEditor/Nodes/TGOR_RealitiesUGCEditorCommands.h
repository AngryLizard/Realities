// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TGOR_RealitiesUGCEditorStyle.h"

class FTGOR_RealitiesUGCEditorCommands : public TCommands<FTGOR_RealitiesUGCEditorCommands>
{
public:

	FTGOR_RealitiesUGCEditorCommands()
		: TCommands<FTGOR_RealitiesUGCEditorCommands>(TEXT("RealitiesUGCEditor"), NSLOCTEXT("Contexts", "RealitiesUGCEditor", "RealitiesUGCEditor Plugin"), NAME_None, FTGOR_RealitiesUGCEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TArray<TSharedPtr<FUICommandInfo>> RegisterUGCCommands(const TArray<TSharedRef<class IPlugin>>& UGCList) const;
	void UnregisterUGCCommands(TArray<TSharedPtr<FUICommandInfo>>& UICommands) const;

public:
	TSharedPtr< FUICommandInfo > CreateUGCAction;
	TSharedPtr< FUICommandInfo > PackageUGCAction;
};