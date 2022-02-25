#pragma once

#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "TGOR_Item.h"
#include "TGOR_InventoryItem.generated.h"

class UTGOR_InventoryStorage;

/**
* The base class for all items that have an item array.
*/
UCLASS(Blueprintable, BlueprintType)
class REALITIES_API UTGOR_InventoryItem : public UTGOR_Item
{
	GENERATED_BODY()

public:
	UTGOR_InventoryItem();
	virtual void BuildStorage(UTGOR_ItemStorage* Storage) override;

	/** Return whether this inventory can hold the specified item type. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual bool AcceptsItem(UTGOR_Item* Content) const;

	/** Return the maximum size of the storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		int32 GetMaxStorageSize() const;

protected:

	/** The maximum amount of items that can be held in the inventory. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		int32 MaxStorageSize;

	/** The type of item that can be stored inside this inventory. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		FTGOR_ItemRestriction AcceptingItem;
};
