#include "TGOR_InventoryItem.h"

#include "Base/Inventory/TGOR_InventoryStorage.h"

UTGOR_InventoryItem::UTGOR_InventoryItem()
	: Super(),
	MaxStorageSize(0)
{
	StorageType = UTGOR_InventoryStorage::StaticClass();
}

int32 UTGOR_InventoryItem::GetMaxStorageSize() const
{
	return MaxStorageSize;
}


UTGOR_Storage* UTGOR_InventoryItem::CreateStorage()
{
	return CreateInventoryStorage();
}


UTGOR_InventoryStorage* UTGOR_InventoryItem::CreateInventoryStorage()
{
	UTGOR_InventoryStorage* Storage = NewObject<UTGOR_InventoryStorage>(GetTransientPackage(), StorageType);
	Storage->SetItem(this);
	return Storage;
}


TSubclassOf<UTGOR_Item> UTGOR_InventoryItem::GetAcceptingItem() const
{
	return AcceptingItem;
}

bool UTGOR_InventoryItem::Equals(const UTGOR_Item* Other) const
{
	if (!UTGOR_Item::Equals(Other)) return false;

	auto InventoryItem = Cast<UTGOR_InventoryItem>(Other);
	if (!IsValid(InventoryItem)) return false;
	if (AcceptingItem != InventoryItem->AcceptingItem) return false;

	return true;
}
