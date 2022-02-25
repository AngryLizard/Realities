// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UI/Inventory/TGOR_ItemContainer.h"
#include "UI/TGOR_Widget.h"

#include "TGOR_ItemArray.generated.h"

//class UTGOR_ItemSlot;
//class UTGOR_ContainerComponent;

/**
 * TGOR_ItemArray automatically creates a grid of itemslots from an inventory and subslot
 */
UCLASS()
class REALITIES_API UTGOR_ItemArray : public UTGOR_ItemContainer
{
	GENERATED_BODY()

public:
	UTGOR_ItemArray();

	virtual void ResetSlots() override;
	virtual void PopulateInventory(UTGOR_ContainerComponent* Inventory, const FTGOR_SlotIndex& Index) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UFUNCTION(BlueprintImplementableEvent)
		void InitialiseGrid(int32 Num);

	UFUNCTION(BlueprintImplementableEvent)
		UTGOR_ItemSlot* InitialiseSlot(int32 Index, APlayerController* Controller);

	UFUNCTION(BlueprintImplementableEvent)
		void ResetGrid();

	////////////////////////////////////////////////////////////////////////////////////////////////////
		
	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_ItemSlot* GetItemslot(const FTGOR_SlotIndex& Index, ETGOR_FetchEnumeration& Branches);
};
