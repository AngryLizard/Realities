// The Gateway of Realities: Planes of Existence.

#include "TGOR_StorageQuery.h"

#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Inventory/TGOR_StorageComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"

#include "Realities/Components/Player/TGOR_UnlockComponent.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_StorageQuery::UTGOR_StorageQuery()
	: Super(),
	HostComponent(nullptr)
{
}

TArray<UTGOR_Content*> UTGOR_StorageQuery::QueryContent() const
{
	TArray<UTGOR_Content*> Content;

	if (IsValid(HostComponent))
	{
		// Get attached items
		const TArray<UTGOR_ItemComponent*> Containers = HostComponent->GetAttachedContainers();
		for (UTGOR_ItemComponent* Container : Containers)
		{
			UTGOR_ItemStorage* Storage = Container->PeekStorage();
			Content.Emplace(Storage->GetItem());
		}
	}

	return Content;
}


void UTGOR_StorageQuery::AssignComponent(UTGOR_StorageComponent* StorageComponent)
{
	HostComponent = StorageComponent;
}
