#include "TGOR_ItemSlotWidget.h"

#include "Base/Inventory/TGOR_InventoryStorage.h"

UTGOR_ItemSlotWidget::UTGOR_ItemSlotWidget()
	: Super()
{
}


void UTGOR_ItemSlotWidget::SelectItem()
{
}

void UTGOR_ItemSlotWidget::CommitItem()
{
}

UTGOR_Storage* UTGOR_ItemSlotWidget::GetItem() const
{
	auto Inventory = GetStorageCasted<UTGOR_InventoryStorage>();
	if (IsValid(ParentContainer) && Inventory)
	{
		return Inventory->GetItemAt(Index);
	}

	return nullptr;
}

/*
bool UTGOR_ItemSlotWidget::PutItem(UTGOR_Storage* NewItem)
{
	auto Inventory = GetStorageCasted<UTGOR_InventoryStorage>();
	if (IsValid(ParentContainer) && Inventory)
	{
		return Inventory->ReplaceItem(NewItem, Index);
	}

	return false;
}
*/
