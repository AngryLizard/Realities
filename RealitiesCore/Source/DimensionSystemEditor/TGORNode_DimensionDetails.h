// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"
#include "UObject/WeakObjectPtrTemplates.h"

class IDetailLayoutBuilder;
class ATGOR_DimensionWorldSettings;

class FTGORNode_DimensionDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

	virtual TSharedRef<SWidget> CreateUpdateDetailsMenu();
	FReply ClickedOnUpdate();
	bool UpdateEnabled() const;

	virtual TSharedRef<SWidget> CreateValidateDetailsMenu();
	FReply ClickedOnValidate();

	virtual TSharedRef<SWidget> CreateConnectionDetailsMenu(TArray<FName> ConnectionNames);
	FReply ClickedOnConnection(FName ConnectionName);

protected:

	/** Cached selected settings */
	TWeakObjectPtr<ATGOR_DimensionWorldSettings> SelectedSettings;
	
};
