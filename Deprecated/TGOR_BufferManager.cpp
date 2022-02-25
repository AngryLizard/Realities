// The Gateway of Realities: Planes of Existence.

#include "TGOR_BufferManager.h"
#include "Base/TGOR_Singleton.h"
#include "Mod/Spawner/Items/TGOR_Item.h"

void UTGOR_BufferManager::FillBufferFromAppearance(UTGOR_Singleton* Singleton, FTGOR_CreatureAppearanceInstance& Appearance, FTGOR_Buffer& Buffer)
{
	Buffer.Type = ETGOR_StorageBufferType::AppearanceInstance;
	FillBufferFromIO<FTGOR_CreatureAppearanceInstance>(Singleton, Appearance, Buffer);
}

void UTGOR_BufferManager::FillAppearanceFromBuffer(UTGOR_Singleton* Singleton, FTGOR_CreatureAppearanceInstance& Appearance, const FTGOR_Buffer& Buffer)
{
	if (Buffer.Type != ETGOR_StorageBufferType::AppearanceInstance)
	{ ERROR("Buffer not an appearance", Error); }
	FillIOFromBuffer<FTGOR_CreatureAppearanceInstance>(Singleton, Appearance, Buffer);
}

void UTGOR_BufferManager::FillBufferFromSetup(UTGOR_Singleton* Singleton, FTGOR_CreatureSetupInstance& Appearance, FTGOR_Buffer& Buffer)
{
	Buffer.Type = ETGOR_StorageBufferType::SetupInstance;
	FillBufferFromIO<FTGOR_CreatureSetupInstance>(Singleton, Appearance, Buffer);
}

void UTGOR_BufferManager::FillSetupFromBuffer(UTGOR_Singleton* Singleton, FTGOR_CreatureSetupInstance& Appearance, const FTGOR_Buffer& Buffer)
{
	if (Buffer.Type != ETGOR_StorageBufferType::SetupInstance)
	{
		ERROR("Buffer not an appearance", Error);
	}
	FillIOFromBuffer<FTGOR_CreatureSetupInstance>(Singleton, Appearance, Buffer);
}


/*void UTGOR_BufferManager::FillBufferFromInventory(UTGOR_Singleton* Singleton, FTGOR_InventoryInstance& Inventory, FTGOR_Buffer& Buffer)
{
	Buffer.Type = ETGOR_StorageBufferType::InventoryInstance;
	FillBufferFromIO<FTGOR_InventoryInstance>(Singleton, Inventory, Buffer);
}

void UTGOR_BufferManager::FillInventoryFromBuffer(UTGOR_Singleton* Singleton, FTGOR_InventoryInstance& Inventory, const FTGOR_Buffer& Buffer)
{
	if (Buffer.Type != ETGOR_StorageBufferType::InventoryInstance)
	{ ERROR("Buffer not an inventory", Error); }
	FillIOFromBuffer<FTGOR_InventoryInstance>(Singleton, Inventory, Buffer);
}


void UTGOR_BufferManager::FillBufferFromInstance(UTGOR_Singleton* Singleton, FTGOR_ItemInstance_OLD& Instance, FTGOR_Buffer& Buffer)
{
	Buffer.Type = ETGOR_StorageBufferType::ItemInstance;
	FillBufferFromIO<FTGOR_ItemInstance_OLD>(Singleton, Instance, Buffer);
}

void UTGOR_BufferManager::FillInstanceFromBuffer(UTGOR_Singleton* Singleton, FTGOR_ItemInstance_OLD& Instance, const FTGOR_Buffer& Buffer)
{
	if (Buffer.Type != ETGOR_StorageBufferType::ItemInstance)
	{ ERROR("Buffer not an instance", Error); }

	FillIOFromBuffer<FTGOR_ItemInstance_OLD>(Singleton, Instance, Buffer);
}*/



