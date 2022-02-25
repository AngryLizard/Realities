
#include "TGOR_BufferComponent.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_BufferComponent::UTGOR_BufferComponent()
	: Super(),
	BufferSize(1)
{
}

void UTGOR_BufferComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UTGOR_BufferComponent::CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const
{
	return Index == FTGOR_ContainerIndex::ZeroContainer && 0 <= Index.Slot && Index.Slot < BufferSize;
}

int32 UTGOR_BufferComponent::GetContainerCapacity(const FTGOR_ContainerIndex& Index) const
{
	return (Index == FTGOR_ContainerIndex::ZeroContainer) ? BufferSize : 0;
}

UTGOR_ItemStorage* UTGOR_BufferComponent::PutIntoBuffer(UTGOR_ItemStorage* Storage)
{
	if (IsValid(Storage))
	{
		// Look for empty slot that supports given item
		for (int32 Slot = 0; Slot < BufferSize; Slot++)
		{
			const FTGOR_SlotIndex SlotIndex(0, Slot);
			if (CanStoreItemAt(SlotIndex, Storage))
			{
				Storage = SwapItemStorage(SlotIndex, Storage);
			}
		}
	}
	return Storage;
}

bool UTGOR_BufferComponent::HasEmptySlot() const
{
	// Look for empty slot
	for (int32 Slot = 0; Slot < BufferSize; Slot++)
	{
		const FTGOR_SlotIndex SlotIndex(0, Slot);
		if (!IsValid(GetItemStorage(SlotIndex)))
		{
			return true;
		}
	}
	return false;
}


UTGOR_ItemStorage* UTGOR_BufferComponent::PickFromBuffer()
{
	// Look for empty slot that supports given item
	for (int32 Slot = 0; Slot < BufferSize; Slot++)
	{
		const FTGOR_SlotIndex SlotIndex(0, Slot);
		UTGOR_ItemStorage* Storage = SwapItemStorage(SlotIndex, Storage);
		if (IsValid(Storage))
		{
			return Storage;
		}
	}
	return nullptr;
}

bool UTGOR_BufferComponent::HasFullSlot() const
{
	// Look for full slot
	for (int32 Slot = 0; Slot < BufferSize; Slot++)
	{
		const FTGOR_SlotIndex SlotIndex(0, Slot);
		if (IsValid(GetItemStorage(SlotIndex)))
		{
			return true;
		}
	}
	return false;
}