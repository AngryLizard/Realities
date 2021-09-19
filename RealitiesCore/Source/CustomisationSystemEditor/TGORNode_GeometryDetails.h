// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class UTGOR_GeometryUserData;

class FTGORNode_GeometryDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

	/** Handle clicking the bake button */
	FReply ClickedOnBake();

	/** Is the convert button enabled */
	bool BakeInputEnabled() const;

	/** Util to get the ProcMeshComp we want to convert */
	USkeletalMesh* GetFirstSelectedSkeleton() const;
	TArray<UTGOR_GeometryUserData*> GetFirstSelectedAssetData() const;

	/** Cached array of selected objects */
	TArray< TWeakObjectPtr<UObject> > SelectedObjectsList;

private:
	
};
