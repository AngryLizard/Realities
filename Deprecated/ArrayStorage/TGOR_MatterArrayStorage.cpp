
#include "TGOR_InventoryStorage.h"

#include "Realities.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance.h"
#include "Mod/Spawner/Items/TGOR_InventoryItem.h"

UTGOR_InventoryStorage::UTGOR_InventoryStorage()
	: Super(),
	ItemRef(nullptr)
{
}
void UTGOR_InventoryStorage::AddItem(UTGOR_Storage* Item)
{
	if (IsSuitable(Item))
	{
		Items.Emplace(Item);
		// TODO: So far most item operations are done on one single item. Sorting every AddItem might become expensive in the future if more items are shuffled at once.
		Items.Sort();
	}
}

/*
bool UTGOR_InventoryStorage::ReplaceItem(UTGOR_Storage* Item, int32 Index)
{
	if (IsIndexValid(Index))
	{
		if (IsSuitable(Item, true))
		{
			if (RemoveAt(Index))
			{
				int32 NewIndex = Items.Insert(Item, Index);
				return NewIndex != INDEX_NONE;
			}
		}
	}

	return false;
}

bool UTGOR_InventoryStorage::SwapItem(UTGOR_Storage* InItem, int32 Index, UTGOR_Storage*& OutItem)
{
	if (IsIndexValid(Index))
	{
		if (IsSuitable(InItem, true))
		{
			OutItem = Items[Index].Storage;
			Items[Index] = InItem;
			return true;
		}
	}

	return false;
}
*/

bool UTGOR_InventoryStorage::RemoveItem(UTGOR_Storage* Item)
{
	int32 Index = INDEX_NONE;
	if (!Items.Find(Item, Index)) return false;

	return RemoveAt(Index);
}

UTGOR_Storage* UTGOR_InventoryStorage::GetItemAt(int32 Index) const
{
	if (IsIndexValid(Index))
	{
		return Items[Index].Storage;
	}

	return nullptr;
}

bool UTGOR_InventoryStorage::IsSuitable(UTGOR_Storage* Item, bool IgnoreSize) const
{
	if (Contains(Item)) // Cycle prevention
	{
		return false;
	}

	// Check boundaries
	if (!IgnoreSize && GetCurrentSize() + 1 > GetMaxSize())
	{
		return false;
	}

	auto InventoryItem = Cast<UTGOR_InventoryItem>(ItemRef);
	if (InventoryItem && IsValid(Item) && IsValid(Item->GetItem()))
	{
		if (Item->GetItem()->IsA(InventoryItem->GetAcceptingItem()))
		{
			return true;
		}
	}

	return false;
}

int32 UTGOR_InventoryStorage::GetItemIndex(UTGOR_Storage* Item) const
{
	int32 Index = Items.Find(Item);
	if (Index != INDEX_NONE)
	{
		return Index;
	}

	return -1;
}

void UTGOR_InventoryStorage::Clear()
{
	Items.Empty();
}

bool UTGOR_InventoryStorage::RemoveAt(int32 Index)
{
	if (IsIndexValid(Index))
	{
		Items.RemoveAt(Index);
		return true;
	}

	return false;
}

void UTGOR_InventoryStorage::Assign(const UTGOR_Storage* Other)
{
	auto OtherInventory = Cast<UTGOR_InventoryStorage>(Other);
	if (IsValid(OtherInventory))
	{
		this->Items = OtherInventory->Items;
	}
}

int32 UTGOR_InventoryStorage::GetMaxSize() const
{
	return ItemRef->GetMaxStorageSize();
}

int32 UTGOR_InventoryStorage::GetCurrentSize() const
{
	return Items.Num();
}

bool UTGOR_InventoryStorage::IsIndexValid(int32 Index) const
{
	return Index >= 0 && Index < GetCurrentSize() && Items.IsValidIndex(Index);
}

UTGOR_Item* UTGOR_InventoryStorage::GetItem() const
{
	return ItemRef;
}

bool UTGOR_InventoryStorage::Equals(const UTGOR_Storage* Other) const
{
	if (!IsValid(Other)) return false;

	if (IsValid(ItemRef) && !ItemRef->Equals(Other->GetItem())) return false;

	auto OtherInventory = Cast<UTGOR_InventoryStorage>(Other);
	for (const auto& ItemThis : Items)
	{
		for (const auto& ItemOther : OtherInventory->Items)
		{
			if (!UTGOR_Item::CompareItemInstances(ItemThis, ItemOther)) return false;
		}
	}

	return true;
}

void UTGOR_InventoryStorage::SetInventoryItem(UTGOR_InventoryItem* NewItem)
{
	ItemRef = NewItem;
}