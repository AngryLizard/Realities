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

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionWorldSettings.h"

#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"


#define LOCTEXT_NAMESPACE "DimensionDetails"

TSharedRef<IDetailCustomization> FTGORNode_DimensionDetails::MakeInstance()
{
	return MakeShareable(new FTGORNode_DimensionDetails);
}

void FTGORNode_DimensionDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	IDetailCategoryBuilder& ProcMeshCategory = DetailBuilder.EditCategory("SkeletalMesh");
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	ProcMeshCategory.AddCustomRow(LOCTEXT("DetailsCategory", "Dimension"), false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
		.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.ToolTipText(LOCTEXT("UpdateTooltip", "Bake vertex data from mesh patch into UserData if available."))
				.OnClicked(this, &FTGORNode_DimensionDetails::ClickedOnUpdate)
				.IsEnabled(this, &FTGORNode_DimensionDetails::UpdateEnabled)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("UpdateButton", "Update"))
				]
			]
		];
}

///////////////////////////////////////////////////////////////////////////////

FReply FTGORNode_DimensionDetails::ClickedOnUpdate()
{
	UWorld* World = GEditor->EditorWorld;
	if (IsValid(World))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				// Set world settings
				Dimension->World = World;
				Dimension->SetIsAbstract(false);

				// Set dimension bounds
				FVector Origin, Extend;
				if (IsValid(World->PersistentLevel) && World->PersistentLevel->LevelBoundsActor.IsValid())
				{
					World->PersistentLevel->LevelBoundsActor->GetActorBounds(false, Origin, Extend);
				}
				else
				{
					// Find level volume
					for (FActorIterator Its(World); Its; ++Its)
					{
						if (Its->IsA(ATGOR_LevelVolume::StaticClass()))
						{
							Cast<ATGOR_LevelVolume>(*Its)->GetActorBounds(false, Origin, Extend);
						}
					}
				}
				Dimension->Bounds = (Extend + Origin.GetAbs()) * 2;


				// Refresh portal list
				Dimension->PublicConnections.Empty();
				for (FActorIterator Its(World); Its; ++Its)
				{
					TArray<UTGOR_ConnectionComponent*> Components;
					Its->GetComponents(Components);
					for (UTGOR_ConnectionComponent* Component : Components)
					{
						if (Component->IsPublic)
						{
							Dimension->PublicConnections.Emplace(Component->GetConnectionName());
						}
					}
				}

				Dimension->MarkPackageDirty();
			}
		}
	}
	return FReply::Handled();
}

bool FTGORNode_DimensionDetails::UpdateEnabled() const
{
	UWorld* World = GEditor->EditorWorld;
	if (IsValid(World))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				return true;
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
