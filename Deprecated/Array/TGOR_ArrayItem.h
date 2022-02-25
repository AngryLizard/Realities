#pragma once

#include "Mod/Spawner/Items/TGOR_Item.h"
#include "TGOR_ArrayItem.generated.h"

class UTGOR_ArrayStorage;

/**
* The base class for all items that have inventory storage to store other items.
*/
UCLASS(Blueprintable, BlueprintType)
class REALITIES_API UTGOR_ArrayItem : public UTGOR_Item
{
	GENERATED_BODY()

public:
	UTGOR_ArrayItem();
	virtual UTGOR_Storage* CreateStorage() override;

	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual UTGOR_ArrayStorage* CreateArrayStorage();

	/** Return whether this inventory can hold an item type. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual bool AcceptsItem(UTGOR_Item* Content) const;

	/** Return the maximum size of the storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		int32 GetMaxStorageSize() const;

protected:

	/** The maximum size of the storage. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		int32 MaxStorageSize;

	/** The type of storage this item has. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		TSubclassOf<UTGOR_ArrayStorage> StorageType;
};
