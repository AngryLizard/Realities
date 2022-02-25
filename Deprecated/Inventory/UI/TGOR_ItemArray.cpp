// The Gateway of Realities: Planes of Existence.

#include "TGOR_ItemArray.h"
#include "Realities.h"

#include "Base/TGOR_Singleton.h"
#include "Base/Instances/Inventory/TGOR_InventoryInstance.h"
#include "Components/Inventory/TGOR_ContainerComponent.h"

UTGOR_ItemArray::UTGOR_ItemArray()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemArray::ResetSlots()
{
	Super::ResetSlots();

	ResetGrid();
}

void UTGOR_ItemArray::PopulateInventory(UTGOR_ContainerComponent* Inventory, const FTGOR_SlotIndex& Index)
{
	
	SINGLETON_CHK
	UTGOR_WorldData* WorldData = Singleton->WorldData;
	FTGOR_InventoryInstance& Instance = Inventory->InventoryInstance;
	
	// Reset slots
	ResetSlots();

	// Get inventoryslot
	int32 Size = Instance.Slots.Num();
	if (!Instance.Slots.IsValidIndex(SubIndex))
	{
		FString Err = FString::FromInt(SubIndex) + " from " + FString::FromInt(Size);
		ERROR(FString("Invalid subindex ") + Err, Error)
	}

	// Get parent
	UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget());
	if (Canvas == nullptr) return;

	// Get player
	APlayerController* Controller = GetOwningPlayer();
	if (Controller == nullptr) return;

	// Initialise
	const FTGOR_InventorySlot& InventorySlot = Instance.Slots[SubIndex];
	const int32 Items = InventorySlot.Items.Num();
	InitialiseGrid(Items);

	// Populate
	for (int i = 0; i < Items; i++)
	{
		// Create itemslot
		UTGOR_ItemSlot* ItemSlot = InitialiseSlot(i, Controller);
		if (!IsValid(ItemSlot))
		{
			ERROR("Couldn't add slot", Error)
		}

		// Add slot to array and initialise
		ItemSlot->AssignSlot(OpenedQuery);
		AddSlot(ItemSlot);
	}
	

	Super::PopulateInventory(Inventory, Index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemSlot* UTGOR_ItemArray::GetItemslot(const FTGOR_SlotIndex& Index, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (ItemSlots.IsValidIndex(Index.Slot)) return(nullptr);

	Branches = ETGOR_FetchEnumeration::Found;
	return(ItemSlots[Index.Slot]);
}