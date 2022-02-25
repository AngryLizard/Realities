// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventoryInstance.h"
#include "Realities.h"

#include "Base/TGOR_Singleton.h"
#include "Mod/Items/TGOR_Item.h"


FTGOR_InventoryRestrict::FTGOR_InventoryRestrict()
	: SlotCount(0), ExtraCount(0)
{}

FTGOR_InventoryRestrict::FTGOR_InventoryRestrict(TSubclassOf<UTGOR_Item> Item, int32 Count)
	: SlotCount(Count), ExtraCount(0), Type(Item)
{}

FTGOR_InventorySlot::FTGOR_InventorySlot()
	: FTGOR_InventorySlot(0)
{}

FTGOR_InventorySlot::FTGOR_InventorySlot(int32 Slots)
{
	for (int i = 0; i < Slots; i++)
		Items.Add(FTGOR_ItemInstance_OLD());
}

bool FTGOR_InventorySlot::Write(UTGOR_Singleton* Singleton, FilePack& Pack) const
{
	SET_FILE_SINGLETON_ARRAY(0, Items)
	return(true);
}

bool FTGOR_InventorySlot::Read(UTGOR_Singleton* Singleton, const FilePack& Pack)
{
	GET_FILE_SINGLETON_ARRAY(0, Items)
	return(true);
}



FTGOR_InventoryInstance::FTGOR_InventoryInstance()
{}

bool FTGOR_InventoryInstance::Write(UTGOR_Singleton* Singleton, FilePack& Pack) const
{
	SET_FILE_SINGLETON_ARRAY(0, Slots)
	return(true);
}

bool FTGOR_InventoryInstance::Read(UTGOR_Singleton* Singleton, const FilePack& Pack)
{
	GET_FILE_SINGLETON_ARRAY(0, Slots)
	return(true);
}


FTGOR_ItemInstance_OLD& FTGOR_InventoryInstance::GetItemInstance(const FTGOR_ItemIndex& Index, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	// Compute Container indexes

	// Make sure indexes are in range
	if (Slots.Num() == 0) Slots.Add(FTGOR_InventorySlot(1));
	if (Index.Slot < 0 || Index.Slot >= Slots.Num()) return(Slots[0].Items[0]);
	if (Index.Item < 0 || Index.Item >= Slots[Index.Slot].Items.Num()) return(Slots[Index.Slot].Items[0]);
	Branches = ETGOR_FetchEnumeration::Found;

	// Get Item instance
	return(Slots[Index.Slot].Items[Index.Item]);
}