// The Gateway of Realities: Planes of Existence.

#include "TGOR_StorageQuery.h"

#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "InventorySystem/Components/TGOR_StorageComponent.h"

#include "InventorySystem/Content/TGOR_Item.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "InventorySystem/Tasks/TGOR_ItemTask.h"

UTGOR_StorageQuery::UTGOR_StorageQuery()
	: Super(),
	HostComponent(nullptr)
{
}

TArray<UTGOR_CoreContent*> UTGOR_StorageQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Content;

	if (IsValid(HostComponent))
	{
		// Get attached items
		const TArray<UTGOR_ItemComponent*> Containers = HostComponent->GetAttachedContainers();
		for (UTGOR_ItemComponent* Container : Containers)
		{
			/*
			UTGOR_ItemStorage* Storage = Container->PeekStorage();
			Content.Emplace(Storage->GetItem());
			*/
		}
	}

	return Content;
}


void UTGOR_StorageQuery::AssignComponent(UTGOR_StorageComponent* StorageComponent)
{
	HostComponent = StorageComponent;
}
