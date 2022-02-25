
#pragma once

#include "TGOR_StorageSlotWidget.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance.h"
#include "TGOR_ItemSlotWidget.generated.h"


/**
 * A slot widget to display an item instance.
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_ItemSlotWidget : public UTGOR_StorageSlotWidget
{
	GENERATED_BODY()

public:
	UTGOR_ItemSlotWidget();
	
	/** Select this item with callback to current query */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void SelectItem();

	/** Commit this item with callback to current query */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void CommitItem();


	/** Replace the item instance in the storage with the given item instance. */
	//UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
	//	virtual bool PutItem(UTGOR_Storage* NewItem); 
	// UPDATE: Inventory operations are done in actions due to networking

	/** Get the item instance that is displayed by this slot. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual UTGOR_Storage* GetItem() const;

private:

};
