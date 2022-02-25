#include "TGOR_ArrayItem.h"

#include "Base/Inventory/ArrayStorage/TGOR_ArrayStorage.h"

UTGOR_ArrayItem::UTGOR_ArrayItem()
	: Super(),
	MaxStorageSize(0)
{
}

UTGOR_Storage* UTGOR_ArrayItem::CreateStorage()
{
	return CreateArrayStorage();
}

UTGOR_ArrayStorage* UTGOR_ArrayItem::CreateArrayStorage()
{
	UTGOR_ArrayStorage* Storage = NewObject<UTGOR_ArrayStorage>(GetTransientPackage(), StorageType);
	Storage->SetItem(this);
	return Storage;
}

int32 UTGOR_ArrayItem::GetMaxStorageSize() const
{
	return MaxStorageSize;
}

bool UTGOR_ArrayItem::AcceptsItem(UTGOR_Item* Content) const
{
	return Content->IsA(AcceptingItem);
}
