// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Mod/Unlocks/Modifiers/Actions/Equipables/TGOR_EquipableAction.h"
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

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called when inventory is to be opened (client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnMenuOpen(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component);

	/** Called when inventory is to be closed (client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnMenuClose(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component);

	/** Ticks and updates menu (client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnMenuTick(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, ETGOR_TerminationEnumeration& Branches);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Takes item from inventory at given index */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* TakeFromInventory(UTGOR_Component* Component, int32 Identifier);

	/** Puts an item into inventory, *preferably* at given index */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void PutIntoInventoryAt(UTGOR_Component* Component, int32 Identifier, UTGOR_ItemStorage* Storage);

	/** Puts an item into inventory */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void PutIntoInventory(UTGOR_Component* Component, UTGOR_ItemStorage* Storage);

	/** Drops an item to the ground at current owner location */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		ATGOR_ItemDropActor* DropItem(UTGOR_Component* Component, UTGOR_ItemStorage* Storage);

	/** Whether this action can access the inventory */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void InventoryBranch(UTGOR_Component* Component, ETGOR_BoolEnumeration& Branches);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Put an item to a given socket */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		ATGOR_ItemDropActor* PutIntoSocket(UTGOR_SocketComponent* Component, UTGOR_ItemStorage* Storage, UTGOR_NamedSocket* Socket);

	/** Take an item from a given socket */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* TakeFromSocket(UTGOR_SocketComponent* Component, UTGOR_NamedSocket* Socket);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////


protected:

private:

};
