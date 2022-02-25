#include "TGOR_InventoryItem.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_InventoryModule.h"

UTGOR_InventoryItem::UTGOR_InventoryItem()
	: Super(),
	MaxStorageSize(0)
{
	Modules.Modules.Add("Storage", UTGOR_InventoryModule::StaticClass());
}


void UTGOR_InventoryItem::BuildStorage(UTGOR_ItemStorage* Storage)
{
	Super::BuildStorage(Storage);

	UTGOR_InventoryModule* Module = Storage->GetModule<UTGOR_InventoryModule>();
	if (IsValid(Module))
	{
		Module->SetRestrictions(AcceptingItem, MaxStorageSize);
	}
}


bool UTGOR_InventoryItem::AcceptsItem(UTGOR_Item* Content) const
{
	return Content->IsSupportedBy(AcceptingItem);
}

int32 UTGOR_InventoryItem::GetMaxStorageSize() const
{
	return MaxStorageSize;
}
