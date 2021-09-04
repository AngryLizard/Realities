// The Gateway of Realities: Planes of Existence.


#include "TGOR_ItemQuery.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Components/TGOR_InventoryComponent.h"
#include "InventorySystem/Content/TGOR_Item.h"

UTGOR_ItemQuery::UTGOR_ItemQuery()
	: Super()
{
}

TArray<UTGOR_CoreContent*> UTGOR_ItemQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Contents;
	if (OwnerComponent.IsValid())
	{
		for (int32 SlotIdentifier : SlotIdentifiers)
		{
			UTGOR_ItemStorage* ItemStorage = OwnerComponent->GetItemStorage(SlotIdentifier);
			if (IsValid(ItemStorage))
			{
				Contents.Emplace(ItemStorage->GetItem());
			}
		}
	}
	return Contents;
}

void UTGOR_ItemQuery::AssignComponent(UTGOR_InventoryComponent* InventoryComponent, const FTGOR_ItemRestriction& Filter)
{
	SlotIdentifiers.Reset();

	OwnerComponent = InventoryComponent;
	SlotFilter = Filter;
}

bool UTGOR_ItemQuery::FilterItem(UTGOR_Item* Item) const
{
	return Item->IsSupportedBy(SlotFilter);
}

void UTGOR_ItemQuery::RefreshItems()
{
	SlotIdentifiers.Reset();
	if (OwnerComponent.IsValid())
	{
		OwnerComponent->ForEachSlot([&](int32 Identifier, UTGOR_ItemStorage* Storage)->bool
		{
			UTGOR_Item* Content = Storage->GetItem();
			if (IsValid(Content) && FilterItem(Content))
			{
				SlotIdentifiers.Emplace(Identifier);
			}
			return true;
		});
	}
}
