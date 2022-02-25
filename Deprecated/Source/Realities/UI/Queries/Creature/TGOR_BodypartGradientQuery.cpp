// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartGradientQuery.h"

#include "Realities/UI/Creature/TGOR_BodypartCustomisationWidget.h"
#include "Realities/UI/Creature/TGOR_BodypartListWidget.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"

#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"


UTGOR_BodypartGradientQuery::UTGOR_BodypartGradientQuery()
: Super()
{
	Required = false;
}

TArray<UTGOR_Content*> UTGOR_BodypartGradientQuery::QueryContent() const
{
	return(MigrateContentArray(PossibleColours));
}

void UTGOR_BodypartGradientQuery::CallContent(int32 Index)
{
	if (IsValid(CallbackWidget) && PossibleColours.IsValidIndex(Index))
	{
		CallbackWidget->SelectCallback(Index);
	}
	Super::CallContent(Index);
}

void UTGOR_BodypartGradientQuery::CallNone()
{
	if (IsValid(CallbackWidget))
	{
		CallbackWidget->NoneCallback();
	}
	Super::CallNone();
}

void UTGOR_BodypartGradientQuery::Commit()
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

void UTGOR_BodypartGradientQuery::InitialiseColourSelection(UTGOR_Colour* Colour)
{
	// Initialise to first entry by default
	InitialiseSelection(0);
	if (IsValid(Colour))
	{
		// Look for already selected colour
		for (int32 Index = 0; Index < PossibleColours.Num(); Index++)
		{
			UTGOR_Colour* PossibleColour = PossibleColours[Index];
			if (PossibleColour == Colour)
			{
				InitialiseSelection(Required ? Index : (Index + 1));
			}
		}
	}
}

void UTGOR_BodypartGradientQuery::SetPossibleColours(UTGOR_BodypartCustomisationWidget* CustomisationWidget)
{
	AssignWorld(CustomisationWidget);
	CallbackWidget = CustomisationWidget;

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(CustomisationWidget->GetOwningPlayer());
	PossibleColours = IsValid(OnlineController) ? OnlineController->GetUnlock()->GetUListFromType<UTGOR_Colour>() : ContentManager->GetTListFromType<UTGOR_Colour>();
}