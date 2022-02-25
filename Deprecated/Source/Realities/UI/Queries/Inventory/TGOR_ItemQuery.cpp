// The Gateway of Realities: Planes of Existence.


#include "TGOR_ItemQuery.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Actions/TGOR_Action.h"

UTGOR_ItemQuery::UTGOR_ItemQuery()
	: Super()
{
}

TArray<UTGOR_Content*> UTGOR_ItemQuery::QueryContent() const
{
	return MigrateContentArray(Items);
}

void UTGOR_ItemQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, const FTGOR_SlotIndex& Index, const FTGOR_ItemRestriction& Filter)
{
	Items.Reset();
	Slots.Reset();

	OwnerComponent = ActionComponent;
	if (OwnerComponent.IsValid() && (*Filter.ItemType))
	{
		ETGOR_BoolEnumeration State;
		ATGOR_Avatar* Avatar = OwnerComponent->GetOwnerAvatar(State);
		UTGOR_Action* Action = OwnerComponent->GetAction(Index);
		if (IsValid(Action) && IsValid(Avatar))
		{
 			UTGOR_InventoryComponent* Inventory = Avatar->GetInventory();
			if (IsValid(Inventory)) // TODO: Check supported items
			{
				Inventory->ForEachSlot([&](const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage)->bool
					{
						UTGOR_Item* Content = Storage->GetItem();
						if (Action->SupportsItem(Content) && Content->IsSupportedBy(Filter) && FilterItem(Storage))
						{
							Items.Emplace(Content);
							Slots.Emplace(Index);
						}
						return true;
					});
			}
		}
	}
}


bool UTGOR_ItemQuery::FilterItem(UTGOR_ItemStorage* Storage) const
{
	return true;
}