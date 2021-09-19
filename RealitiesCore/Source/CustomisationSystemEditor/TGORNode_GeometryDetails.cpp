// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGORNode_GeometryDetails.h"

#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"

#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"

#include "CustomisationSystem/UserData/TGOR_GeometryUserData.h"

#define LOCTEXT_NAMESPACE "SkeletalMergeDetails"

TSharedRef<IDetailCustomization> FTGORNode_GeometryDetails::MakeInstance()
{
	return MakeShareable(new FTGORNode_GeometryDetails);
}

void FTGORNode_GeometryDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	IDetailCategoryBuilder& ProcMeshCategory = DetailBuilder.EditCategory("SkeletalMesh");

	const FText BakePatchesText = LOCTEXT("Bake", "Bake");
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	ProcMeshCategory.AddCustomRow(BakePatchesText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
	.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
		.ToolTipText(LOCTEXT("BakeTooltip", "Bake vertex data from mesh patch into UserData if available."))
		.OnClicked(this, &FTGORNode_GeometryDetails::ClickedOnBake)
		.IsEnabled(this, &FTGORNode_GeometryDetails::BakeInputEnabled)
		.Content()
		[
			SNew(STextBlock)
			.Text(BakePatchesText)
		]
		];
}

USkeletalMesh* FTGORNode_GeometryDetails::GetFirstSelectedSkeleton() const
{
	// Find first selected valid skeleton
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Object.Get());
		if (SkeletalMesh != nullptr)
		{
			if (IsValid(SkeletalMesh->GetAssetUserData<UTGOR_GeometryUserData>()))
			{
				return SkeletalMesh;
			}
		}
	}
	return nullptr;
}

TArray<UTGOR_GeometryUserData*> FTGORNode_GeometryDetails::GetFirstSelectedAssetData() const
{
	TArray<UTGOR_GeometryUserData*> Meshes;

	USkeletalMesh* Mesh = GetFirstSelectedSkeleton();
	if (IsValid(Mesh))
	{
		const TArray<UAssetUserData*>* UserDatas = Mesh->GetAssetUserDataArray();
		for (UAssetUserData* UserData : *UserDatas)
		{
			UTGOR_GeometryUserData* GeometryUserData = Cast<UTGOR_GeometryUserData>(UserData);
			if (IsValid(GeometryUserData))
			{
				Meshes.Emplace(GeometryUserData);
			}
		}
	}
	return Meshes;
}

///////////////////////////////////////////////////////////////////////////////

bool FTGORNode_GeometryDetails::BakeInputEnabled() const
{
	TArray<UTGOR_GeometryUserData*> AssetUserDatas = GetFirstSelectedAssetData();
	if (AssetUserDatas.Num() > 0)
	{
		return true;
	}
	return false;
	//return AssetDataEnabled() && _skeletalMesh.IsValid() && (!_bakeName.IsNone() || !_bakeName.IsEqual(FName("")));
}

///////////////////////////////////////////////////////////////////////////////

FReply FTGORNode_GeometryDetails::ClickedOnBake()
{
	USkeletalMesh* Mesh = GetFirstSelectedSkeleton();
	if (IsValid(Mesh))
	{
		TArray<UTGOR_GeometryUserData*> AssetUserDatas = GetFirstSelectedAssetData();
		for (UTGOR_GeometryUserData* AssetUserData : AssetUserDatas)
		{
			AssetUserData->BakeData(Mesh);
		}
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
