// The Gateway of Realities: Planes of Existence.


#include "TGOR_RecipeQuery.h"
#include "InventorySystem/Components/TGOR_MatterComponent.h"

#include "KnowledgeSystem/Components/TGOR_UnlockComponent.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "InventorySystem/Content/TGOR_Matter.h"
#include "InventorySystem/Content/TGOR_Segment.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

UTGOR_RecipeQuery::UTGOR_RecipeQuery()
	: Super(),
	HostComponent(nullptr)
{
}

TArray<UTGOR_CoreContent*> UTGOR_RecipeQuery::QueryContent() const
{
	return MigrateContentArray(Recipes);
}


void UTGOR_RecipeQuery::AssignComponent(APlayerController* OwningPlayer, UTGOR_MatterComponent* MatterComponent, ETGOR_RecipeEnumeration Match)
{
	HostComponent = MatterComponent;
	RecipeMatch = Match;


	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Clear state
	Recipes.Reset();

	// Get inventory of provided ActionComponent
	if (IsValid(OwningPlayer) && IsValid(HostComponent))
	{
		TMap<UTGOR_Matter*, int32> Composition;
		HostComponent->MatterContainers.AddComposition(UTGOR_Segment::StaticClass(), Composition);

		// Get unlocked items
		UTGOR_UnlockComponent* UnlockComponent = OwningPlayer->FindComponentByClass<UTGOR_UnlockComponent>();
		const TArray<UTGOR_Item*> Items = ContentManager->GetTListFromType<UTGOR_Item>();// TODO: IsValid(UnlockComponent) ? UnlockComponent->GetUListFromType<UTGOR_Item>() : 
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
