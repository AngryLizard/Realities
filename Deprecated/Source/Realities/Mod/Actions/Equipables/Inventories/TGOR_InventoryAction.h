// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Mod/Actions/Equipables/TGOR_EquipableAction.h"
#include "TGOR_InventoryAction.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_InventoryTabMenu;
class UTGOR_InventoryStorage;
class UTGOR_ItemStorage;

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_InventoryAction : public UTGOR_EquipableAction
{
	GENERATED_BODY()

public:
	UTGOR_InventoryAction();

	virtual void ForceEquip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) override;
	virtual bool Operate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime) override;
	virtual void ForceUnequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called when inventory is to be opened (client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void OnMenuOpen(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);

	/** Called when inventory is to be closed (client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void OnMenuClose(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component);

	/** Ticks and updates menu (client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void OnMenuTick(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, ETGOR_TerminationEnumeration& Branches);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////


	/** Takes item from inventory at given index */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* TakeFromInventory(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Index);

	/** Puts an item into inventory, *preferably* at given index */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void PutIntoInventoryAt(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage);

	/** Puts an item into inventory */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void PutIntoInventory(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* Storage);

	/** Drops an item to the ground at current owner location */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		ATGOR_ItemDropActor* DropItem(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* Storage);

	/** Whether this action can access the inventory */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void InventoryBranch(UTGOR_ActionComponent* Component, ETGOR_BoolEnumeration& Branches);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Tab widget to be displayed when opened */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		TSubclassOf<UTGOR_InventoryTabMenu> Tab;

protected:

private:

};
