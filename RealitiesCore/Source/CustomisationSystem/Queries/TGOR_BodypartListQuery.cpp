// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartListQuery.h"

#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Modular.h"
#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"
#include "CustomisationSystem/UI/TGOR_BodypartSelectionWidget.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "KnowledgeSystem/Components/TGOR_UnlockComponent.h"

UTGOR_BodypartListQuery::UTGOR_BodypartListQuery()
	: Super(),
	CustomisationNodeIndex(INDEX_NONE),
	OwnerComponent(nullptr)
{

}

TArray<UTGOR_CoreContent*> UTGOR_BodypartListQuery::QueryContent() const
{
	return(MigrateContentArray(PossibleBodyparts));
}


void UTGOR_BodypartListQuery::CommitNone()
{
	RemoveBodypart();
	Super::CommitNone();
}

void UTGOR_BodypartListQuery::CommitContent(int32 Index)
{
	if (PossibleBodyparts.IsValidIndex(Index))
	{
		ReplaceBodypart(PossibleBodyparts[Index]);
	}

	Super::CommitContent(Index);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_BodypartListQuery::RemoveBodypart()
{
	if (IsValid(OwnerComponent))
	{
		// Get bodypart if available
		const int32 NodeIndex = OwnerComponent->GetFirstChildFromType(CustomisationNodeIndex, BodypartType);
		if (NodeIndex != INDEX_NONE)
		{
			OwnerComponent->RemoveChild(CustomisationNodeIndex, NodeIndex);

			// Update parent
			OwnerComponent->UpdateAppearance(CustomisationNodeIndex);
		}
	}
}


void UTGOR_BodypartListQuery::ReplaceBodypart(UTGOR_Bodypart* Content)
{
	if (IsValid(OwnerComponent))
	{
		// Change or add bodypart to node
		const int32 NodeIndex = OwnerComponent->GetFirstChildFromType(CustomisationNodeIndex, BodypartType);
		if (NodeIndex != INDEX_NONE)
		{
			OwnerComponent->ChangeBodypart(NodeIndex, Content);
		}
		else
		{
			// Create bodypart and open
			CustomisationNodeIndex = OwnerComponent->AddChildBodypart(CustomisationNodeIndex, Content);
		}

		// Update parent
		OwnerComponent->UpdateAppearance(CustomisationNodeIndex);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_BodypartListQuery::InitialiseBodypartSelection(UTGOR_Bodypart* Bodypart)
{
	// Initialise to first entry by default
	InitialiseSelection(0);
	if (IsValid(Bodypart))
	{
		// Look for already selected bodypart
		for (int32 Index = 0; Index < PossibleBodyparts.Num(); Index++)
		{
			UTGOR_Bodypart* PossibleBodypart = PossibleBodyparts[Index];
			if (PossibleBodypart == Bodypart)
			{
				InitialiseSelection(Required ? Index : (Index + 1));
				return;
			}
		}
	}
}

void UTGOR_BodypartListQuery::SetPossibleBodyparts(APlayerController* OwningPlayer, UTGOR_CustomisationComponent* Component, int32 NodeIndex, TSubclassOf<UTGOR_Bodypart> Type, bool IsRequired)
{
	if (!IsValid(Component)) ERROR("No Parent defined", Error);
	AssignWorld(Component);

	CustomisationNodeIndex = NodeIndex;
	OwnerComponent = Component;
	BodypartType = Type;
	Required = IsRequired;

	if (IsValid(OwnerComponent))
	{
		const FTGOR_BodypartInstance& Instance = OwnerComponent->CurrentAppearance.Bodyparts[CustomisationNodeIndex];
		if (IsValid(Instance.Content))
		{
			PossibleBodyparts = Component->Modular->Instanced_BodypartInsertions.GetListOfType(BodypartType);

			if (IsValid(OwningPlayer))
			{
				UTGOR_UnlockComponent* Unlock = OwningPlayer->FindComponentByClass<UTGOR_UnlockComponent>();
				if (IsValid(Unlock))
				{
					//TODO: PossibleBodyparts = Unlock->FilterU<UTGOR_Bodypart>(PossibleBodyparts);
				}
			}
		}
	}
}
