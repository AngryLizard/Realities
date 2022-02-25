// The Gateway of Realities: Planes of Existence.


#include "TGOR_RecipeQuery.h"
#include "Realities/Components/Inventory/TGOR_MatterComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"

#include "Realities/Components/Player/TGOR_UnlockComponent.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_RecipeQuery::UTGOR_RecipeQuery()
	: Super(),
	OwnerComponent(nullptr),
	HostComponent(nullptr)
{
}

TArray<UTGOR_Content*> UTGOR_RecipeQuery::QueryContent() const
{
	return MigrateContentArray(Recipes);
}


void UTGOR_RecipeQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_MatterComponent* MatterComponent, ETGOR_RecipeEnumeration Match)
{
	OwnerComponent = ActionComponent;
	HostComponent = MatterComponent;
	RecipeMatch = Match;


	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Clear state
	Recipes.Reset();

	// Get inventory of provided ActionComponent
	if (IsValid(OwnerComponent) && IsValid(HostComponent))
	{
		TMap<UTGOR_Matter*, int32> Composition = HostComponent->GetComposition();

		// Get unlocked items
		ATGOR_Pawn* Pawn = ActionComponent->GetOwnerPawn();
		ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(Pawn->Controller);
		const TArray<UTGOR_Item*> Items = IsValid(OnlineController) ? OnlineController->GetUnlock()->GetUListFromType<UTGOR_Item>() : ContentManager->GetTListFromType<UTGOR_Item>();
		for (UTGOR_Item* Item : Items)
		{
			// Split into categories
			if (Item->CanAssembleWith(Composition))
			{
				if (Match == ETGOR_RecipeEnumeration::FullMatch) Recipes.Emplace(Item);
			}
			else if (Item->IsCompatibleWith(Composition))
			{
				if (Match == ETGOR_RecipeEnumeration::SomeMatch) Recipes.Emplace(Item);
			}
			else
			{
				if(Match == ETGOR_RecipeEnumeration::NoMatch) Recipes.Emplace(Item);
			}

		}
	}
}
