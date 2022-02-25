// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartSkinQuery.h"

#include "Realities/UI/Creature/TGOR_BodypartCustomisationWidget.h"
#include "Realities/UI/Creature/TGOR_BodypartListWidget.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"

#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"

UTGOR_BodypartSkinQuery::UTGOR_BodypartSkinQuery()
	: Super()
{

}

TArray<UTGOR_Content*> UTGOR_BodypartSkinQuery::QueryContent() const
{
	return(MigrateContentArray(PossibleSkins));
}

void UTGOR_BodypartSkinQuery::CallContent(int32 Index)
{
	if (IsValid(CallbackWidget) && PossibleSkins.IsValidIndex(Index))
	{
		CallbackWidget->SelectCallback(Index);
	}
	Super::CallContent(Index);
}

void UTGOR_BodypartSkinQuery::Commit()
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

void UTGOR_BodypartSkinQuery::InitialiseSkinSelection(UTGOR_Skin* Skin)
{
	// Initialise to first entry by default
	InitialiseSelection(0);
	if (IsValid(Skin))
	{
		// Look for already selected skin
		for (int32 Index = 0; Index < PossibleSkins.Num(); Index++)
		{
			UTGOR_Skin* PossibleSkin = PossibleSkins[Index];
			if (PossibleSkin == Skin)
			{
				InitialiseSelection(Required ? Index : (Index + 1));
			}
		}
	}
}

void UTGOR_BodypartSkinQuery::SetPossibleSkins(UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart)
{
	AssignWorld(CustomisationWidget);
	CallbackWidget = CustomisationWidget;

	PossibleSkins = Bodypart->GetIListFromType<UTGOR_Skin>(UTGOR_Skin::StaticClass());

	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(CustomisationWidget->GetOwningPlayer());
	if (IsValid(OnlineController))
	{
		PossibleSkins = OnlineController->GetUnlock()->FilterU<UTGOR_Skin>(PossibleSkins);
	}
}