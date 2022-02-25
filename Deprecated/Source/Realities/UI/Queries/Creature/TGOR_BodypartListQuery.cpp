// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartListQuery.h"

#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/UI/Creature/TGOR_BodypartListWidget.h"
#include "Realities/UI/Creature/TGOR_BodypartSelectionWidget.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Creature/TGOR_BodypartNode.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"

UTGOR_BodypartListQuery::UTGOR_BodypartListQuery()
	: Super()
{

}

TArray<UTGOR_Content*> UTGOR_BodypartListQuery::QueryContent() const
{
	return(MigrateContentArray(PossibleBodyparts));
}


void UTGOR_BodypartListQuery::CallContent(int32 Index)
{
	if (IsValid(CallbackWidget) && PossibleBodyparts.IsValidIndex(Index))
	{
		CallbackWidget->ReplaceBodypart(PossibleBodyparts[Index]);
	}
	Super::CallContent(Index);
}

void UTGOR_BodypartListQuery::CallNone()
{
	CallbackWidget->RemoveBodypart();
}


void UTGOR_BodypartListQuery::Commit()
{
	Super::Commit();
	if (IsValid(CallbackWidget))
	{
		UTGOR_BodypartListWidget* BodypartListWidget = CallbackWidget->GetBodypartList();
		if (IsValid(BodypartListWidget))
		{
			UTGOR_BodypartNode* BodypartNode = CallbackWidget->GetBodypartNode();

			BodypartListWidget->CloseContentList();
			BodypartListWidget->SetupFromBodypart(BodypartNode);
		}
	}
}

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

void UTGOR_BodypartListQuery::SetPossibleBodyparts(UTGOR_BodypartSelectionWidget* BodypartSelectionWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Bodypart> Class, bool IsRequired)
{
	if (!IsValid(BodypartSelectionWidget)) ERROR("No callback defined", Error)
	if (!IsValid(Bodypart)) ERROR("No Bodypart defined", Error)
	
	AssignWorld(Bodypart);

	CallbackWidget = BodypartSelectionWidget;
	PossibleBodyparts = Bodypart->GetIListFromType<UTGOR_Bodypart>(Class);

	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(BodypartSelectionWidget->GetOwningPlayer());
	if (IsValid(OnlineController))
	{
		PossibleBodyparts = OnlineController->GetUnlock()->FilterU<UTGOR_Bodypart>(PossibleBodyparts);
	}

	Required = IsRequired;
}
