#pragma once

#include "CoreMinimal.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance.h"

#include "Base/Inventory/TGOR_Storage.h"
#include "TGOR_InventoryStorage.generated.h"

/**
 * The base class of an inventory. Can be inherited to create extra functionallity.
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_InventoryStorage : public UTGOR_Storage
{
	GENERATED_BODY()

	friend class UTGOR_InventoryItem;

public:
	UTGOR_InventoryStorage();

	/** Adds the item to the item list and sorts the list by storage index. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual void AddItem(UTGOR_Storage* Item);

	// UPDATE: Since this list needs to be ordered there is no performance improvement from doing an index swap
	/** First removes the item in the current slot and put the new inside. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
	//	virtual bool ReplaceItem(UTGOR_Storage* Item, int32 Index);

	/** Put the new item in the desired slot and return the item in the old slot. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
	//	virtual bool SwapItem(UTGOR_Storage* InItem, int32 Index, UTGOR_Storage*& OutItem); 

	/** Removes and item and destroys it. There is also an indexed version. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual bool RemoveItem(UTGOR_Storage* Item);

	/** Get the item under the given index. Returns nullptr on failure, */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual UTGOR_Storage* GetItemAt(int32 Index) const;

	/** Base checks for an item. Will called before every item insertation. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual bool IsSuitable(UTGOR_Storage* Item, bool IgnoreSize = false) const;

	/** Get a index out of the given item. Returns INDEX_NONE on failure. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual int32 GetItemIndex(UTGOR_Storage* Item) const;

	/** Remove the element at the given position. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual bool RemoveAt(int32 Index) { unimplemented() return false; }

	/** Get the current size of the storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual int32 GetCurrentSize() const { unimplemented() return 0; }

	/** Get the maximum size of the storage container. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual int32 GetMaxSize() const { unimplemented() return 0; }

	/** Test if the given index is in the range of the storage container. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual bool IsIndexValid(int32 Index) const { unimplemented() return false; }


	/* UTGOR_Storage */

	virtual void Clear() override;
	virtual void Assign(const UTGOR_Storage* Other) override;
	virtual bool Equals(const UTGOR_Storage* Other) const override;

	virtual UTGOR_Item* GetItem() const override;

protected:

	/** Set ItemRef. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
	void SetItem(UTGOR_InventoryItem* NewItem);

private:

	/** The inventory item of this storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage")
		UTGOR_InventoryItem* ItemRef;

	/** The list of stored items. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage")
		TArray<FTGOR_ItemInstance> Items;
};