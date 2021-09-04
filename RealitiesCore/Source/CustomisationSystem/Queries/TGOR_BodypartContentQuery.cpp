// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartContentQuery.h"

#include "CustomisationSystem/UI/TGOR_BodypartCustomisationWidget.h"
#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"
#include "KnowledgeSystem/Components/TGOR_UnlockComponent.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Palette.h"
#include "CustomisationSystem/Content/TGOR_Mask.h"
#include "CustomisationSystem/Content/TGOR_Skin.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_BodypartContentQuery::UTGOR_BodypartContentQuery()
	: Super()
{

}

TArray<UTGOR_CoreContent*> UTGOR_BodypartContentQuery::QueryContent() const
{
	return MigrateContentArray(PossibleContent);
}

void UTGOR_BodypartContentQuery::CommitNone()
{
	if (IsValid(CallbackWidget))
	{
		CallbackWidget->NoneCallback();
	}
	Super::CommitNone();
}

void UTGOR_BodypartContentQuery::CommitContent(int32 Index)
{
	if (IsValid(CallbackWidget) && PossibleContent.IsValidIndex(Index))
	{
		CallbackWidget->ContentCallback(PossibleContent[Index]);
	}
	Super::CommitContent(Index);
}

void UTGOR_BodypartContentQuery::InitialiseContentSelection(UTGOR_CoreContent* Content)
{
	// Initialise to first entry by default
	InitialiseSelection(0);
	if (IsValid(Content))
	{
		// Look for already selected skin
		for (int32 Index = 0; Index < PossibleContent.Num(); Index++)
		{
			if (PossibleContent[Index] == Content)
			{
				InitialiseSelection(Required ? Index : (Index + 1));
			}
		}
	}
}

void UTGOR_BodypartContentQuery::SetPossibleSkins(APlayerController* OwningPlayer, UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Skin> Typefilter)
{
	AssignWorld(CustomisationWidget);
	CallbackWidget = CustomisationWidget;
	if (IsValid(Bodypart))
	{
		PossibleContent = Bodypart->Instanced_SkinInsertions.GetListOfType<UTGOR_CoreContent>(Typefilter);
	}
	else
	{
		SINGLETON_CHK;
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		PossibleContent = ContentManager->GetTListFromType<UTGOR_CoreContent>(Typefilter);
	}

	if (IsValid(OwningPlayer))
	{
		UTGOR_UnlockComponent* Unlock = OwningPlayer->FindComponentByClass<UTGOR_UnlockComponent>();
		if (IsValid(Unlock))
		{
			//TODO: PossibleContent = Unlock->FilterU<UTGOR_CoreContent>(PossibleContent);
		}
	}

	Required = true;
}

void UTGOR_BodypartContentQuery::SetPossibleMasks(APlayerController* OwningPlayer, UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Mask> Typefilter)
{
	AssignWorld(CustomisationWidget);
	CallbackWidget = CustomisationWidget;

	if (IsValid(Bodypart))
	{
		PossibleContent = Bodypart->Instanced_MaskInsertions.GetListOfType<UTGOR_CoreContent>(Typefilter);
	}
	else
	{
		SINGLETON_CHK;
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		PossibleContent = ContentManager->GetTListFromType<UTGOR_CoreContent>(Typefilter);
	}

	if (IsValid(OwningPlayer))
	{
		UTGOR_UnlockComponent* Unlock = OwningPlayer->FindComponentByClass<UTGOR_UnlockComponent>();
		if (IsValid(Unlock))
		{
			//TODO: PossibleContent = Unlock->FilterU<UTGOR_Unlock>(PossibleContent);
		}
	}

	Required = true;
}

void UTGOR_BodypartContentQuery::SetPossiblePalettes(APlayerController* OwningPlayer, UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Palette> Typefilter)
{
	AssignWorld(CustomisationWidget);
	CallbackWidget = CustomisationWidget;

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	PossibleContent = ContentManager->GetTListFromType<UTGOR_CoreContent>(Typefilter);

	if (IsValid(OwningPlayer))
	{
		UTGOR_UnlockComponent* Unlock = OwningPlayer->FindComponentByClass<UTGOR_UnlockComponent>();
		if (IsValid(Unlock))
		{
			//TODO: PossibleContent = Unlock->FilterU<UTGOR_Unlock>(PossibleContent);
		}
	}

	Required = true;
}
