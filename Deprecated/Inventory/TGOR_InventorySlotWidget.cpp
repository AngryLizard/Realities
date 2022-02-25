// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventorySlotWidget.h"
#include "Components/System/Actions/TGOR_ActionComponent.h"
#include "Components/Inventory/TGOR_InventoryComponent.h"
#include "Base/Inventory/TGOR_InventoryStorage.h"
#include "UI/Queries/Inventory/TGOR_ItemQuery.h"

UTGOR_InventorySlotWidget::UTGOR_InventorySlotWidget()
	: ItemQuery(nullptr)
{
}


void UTGOR_InventorySlotWidget::Assign(UTGOR_ItemQuery* Query, int32 Item)
{
	ItemQuery = Query;
	ItemIndex = Item;
	if (IsValid(ItemQuery) && IsValid(ItemQuery->OwnerComponent) && ItemQuery->Slots.IsValidIndex(Item))
	{
		ATGOR_Avatar* Avatar = ItemQuery->OwnerComponent->GetOwnerAvatar();
		if (IsValid(Avatar) && IsValid(Avatar->GetInventory()))
		{
			UTGOR_InventoryComponent* Inventory = Avatar->GetInventory();
			UTGOR_Storage* Storage = Inventory->GetSlot(ItemQuery->Slots[Item]);
			if (IsValid(Storage))
			{
				VisualiseItem(Storage->GetItem());
			}
		}
	}
}

void UTGOR_InventorySlotWidget::SelectItem()
{
	if (IsValid(ItemQuery))
	{
		ItemQuery->CallIndex(ItemIndex);
	}
}

void UTGOR_InventorySlotWidget::CommitItem()
{
	if (IsValid(ItemQuery))
	{
		ItemQuery->Commit();
	}
}
