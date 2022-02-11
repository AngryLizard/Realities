// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGORNode_DimensionDetails.h"
#include "Engine/World.h"
#include "Engine.h"
#include "EngineUtils.h"

#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Misc/MessageDialog.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionWorldSettings.h"

#include "DimensionSystemEditor/TGOR_DimensionEditorFunctionLibrary.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "DimensionDetails"

TSharedRef<IDetailCustomization> FTGORNode_DimensionDetails::MakeInstance()
{
	return MakeShareable(new FTGORNode_DimensionDetails);
}

void FTGORNode_DimensionDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("!TGOR Dimension");

	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList = DetailBuilder.GetSelectedObjects();
	if (SelectedObjectsList.Num() > 0)
	{
		SelectedSettings = Cast<ATGOR_DimensionWorldSettings>(SelectedObjectsList[0].Get());
	}

	if (SelectedSettings.IsValid())
	{
		Category.AddCustomRow(LOCTEXT("DetailsCategoryButtons", "Dimension"), false)
			.NameContent()
			[
				SNew(STextBlock).Text(LOCTEXT("DetailsButtonsName", "Dimension"))
			]
			.ValueContent()
			.VAlign(VAlign_Center)
			.MaxDesiredWidth(250)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot() [ CreateUpdateDetailsMenu() ]
				+ SHorizontalBox::Slot() [ CreateValidateDetailsMenu() ]
			];

		if (*SelectedSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = SelectedSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				TArray<FName> ConnectionNames = UTGOR_DimensionEditorFunctionLibrary::GetConnectionListFromWorld(SelectedSettings.Get());

				Category.AddCustomRow(LOCTEXT("DetailsCategoryConnections", "Connections"), false)
					.NameContent()
					[
						SNew(STextBlock).Text(LOCTEXT("DetailsConnectionsName", "Connections"))
					]
					.ValueContent()
					.VAlign(VAlign_Center)
					.MaxDesiredWidth(250)
					[
						SNew(SComboButton)
						.OnGetMenuContent(this, &FTGORNode_DimensionDetails::CreateConnectionDetailsMenu, ConnectionNames)
						.ContentPadding(FMargin(2.0f, 2.0f))
						.ToolTipText(LOCTEXT("DetailsConnectionsTooltip", "Available connections"))
						.ButtonContent()
						[
							SNew(STextBlock).Text(LOCTEXT("DetailsConnectionsButton", "Select"))
						]
					];
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> FTGORNode_DimensionDetails::CreateUpdateDetailsMenu()
{
	return SNew(SButton)
		.ToolTipText(LOCTEXT("UpdateTooltip", "Bake vertex data from mesh patch into UserData if available."))
		.OnClicked(this, &FTGORNode_DimensionDetails::ClickedOnUpdate)
		.IsEnabled(this, &FTGORNode_DimensionDetails::UpdateEnabled)
		.Content()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("UpdateButton", "Update"))
		];
}

FReply FTGORNode_DimensionDetails::ClickedOnUpdate()
{
	if (SelectedSettings.IsValid())
	{
		if(*SelectedSettings->Dimension)
		{
			UTGOR_Dimension* Dimension = SelectedSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension) && Dimension->IsAbstract())
			{
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Error_WrongDimension", "Assigned dimension needs to be non-abstract."));
				return FReply::Handled();
			}
		}

		UTGOR_DimensionEditorFunctionLibrary::AssignCurrentWorldToDimension(SelectedSettings.Get());
		UTGOR_DimensionEditorFunctionLibrary::AssignCurrentBoundsToDimension(SelectedSettings.Get());
		UTGOR_DimensionEditorFunctionLibrary::EnsureUniqeIdentifiers(SelectedSettings.Get());
		UTGOR_DimensionEditorFunctionLibrary::UpdateConnectionList(SelectedSettings.Get());
	}
	return FReply::Handled();
}

bool FTGORNode_DimensionDetails::UpdateEnabled() const
{
	if (SelectedSettings.IsValid() && *SelectedSettings->Dimension)
	{
		// Get dimension
		UTGOR_Dimension* Dimension = SelectedSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
		if (IsValid(Dimension))
		{
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> FTGORNode_DimensionDetails::CreateValidateDetailsMenu()
{
	return SNew(SBox)
		.WidthOverride(280)
		[
			SNew(SButton)
			.ToolTipText(LOCTEXT("ValidateTooltip", "Validate all dimension components"))
			.OnClicked(this, &FTGORNode_DimensionDetails::ClickedOnValidate)
			.Content()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ValidateButton", "Validate"))
			]
		];
}

FReply FTGORNode_DimensionDetails::ClickedOnValidate()
{
	if (SelectedSettings.IsValid())
	{
		TArray<AActor*> Actors = UTGOR_DimensionEditorFunctionLibrary::GetMissingIdentifierComponent(SelectedSettings.Get());

		if (Actors.Num() > 0)
		{
			EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::OkCancel, LOCTEXT("Error_MissingIdentifierFound", "Not all dimension interfaces are owned by an actor without identity component. Bad actors will be selected."));
			if (ReturnType == EAppReturnType::Ok)
			{
				for (AActor* Actor : Actors)
				{
					GEditor->SelectActor(Actor, true, true);
				}
			}
		}
	}
	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> FTGORNode_DimensionDetails::CreateConnectionDetailsMenu(TArray<FName> ConnectionNames)
{
	TSharedRef<SVerticalBox> ConnectionBox = SNew(SVerticalBox);
	for (const FName& ConnectionName : ConnectionNames)
	{
		ConnectionBox->AddSlot()
		[
			SNew(SButton)
			.ToolTipText(LOCTEXT("ConnectionTooltip", "Select Connection."))
			.OnClicked(this, &FTGORNode_DimensionDetails::ClickedOnConnection, ConnectionName)
			.Content()
			[
				SNew(STextBlock)
				.Text(FText::FromName(ConnectionName))
			]
		];
	}

	return SNew(SBorder)
		.Padding(FMargin(2.f, 2.f, 2.f, 2.f))
		[
			ConnectionBox
		];
}

FReply FTGORNode_DimensionDetails::ClickedOnConnection(FName ConnectionName)
{
	if (SelectedSettings.IsValid())
	{
		UTGOR_ConnectionComponent* Component = UTGOR_DimensionEditorFunctionLibrary::GetConnectionFromWorld(ConnectionName, SelectedSettings.Get());
		if (IsValid(Component))
		{
			GEditor->SelectActor(Component->GetOwner(), true, true);
		}
	}

	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
