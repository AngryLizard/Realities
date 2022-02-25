
#pragma once

#include "CoreMinimal.h"
#include "UI/TGOR_ButtonArray.h"
#include "Components/Inventory/TGOR_ItemComponent.h"

#include "TGOR_StorageContainerMenu.generated.h"

/**
 * A base class to display the storage of an item instance.
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_StorageContainerMenu : public UTGOR_ButtonArray
{
	GENERATED_BODY()

	friend class UTGOR_StorageSlotWidget;

public:
	UTGOR_StorageContainerMenu();

	/** 
	* Initialize the container for displaying the item instance storage. 
	*
	* @param SlotWidget The widget that is created to display a slot of the storage.
	* @param NewComponent The item component of which the contents are displayed.
	* @param ContainerWidget A panel widget to show all created slots. Can bu null. Will be automatically updated on storage changes.
	*/
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual void Initzialize(TSubclassOf<UTGOR_StorageSlotWidget> SlotWidget, UTGOR_ItemComponent* NewComponent, UPanelWidget* NewContainerWidget);

	/** Rebuild all slots to match the elements in the storage. Is also updating the panel widget, if set. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual void RebuildSlots();

	/** Removes the slot from the container, does not remove the element from the storage. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual bool RemoveSlotAt(int32 Index);

	/** Removes the slot from the container and the element from the storage. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual bool RemoveItemAndSlotAt(int32 Index);

	// TODO: Make slot return const?
	/** Return the slot under the given index. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		UTGOR_StorageSlotWidget* GetSlotAt(int32 Index);

	/** Return the item storage of the item instance that is displaying it's contents in here. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		UTGOR_Storage* GetStorage();

	/** Return all slots created by this container. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage UI", Meta = (Keywords = "C++"))
		const TArray<UTGOR_StorageSlotWidget*>& GetSlots() const;
	
private:
	/** All slot widgets created by this container for the storage of the item instance. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage UI")
		TArray<UTGOR_StorageSlotWidget*> Slots;

	/** The widget class that is used to create the slot widgets. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage UI")
		TSubclassOf<UTGOR_StorageSlotWidget> SlotWidgetClass;

	/** The item component that is used to display it's storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage UI")
		UTGOR_ItemComponent* ItemComponent;

	/** The panel widget that is updated when the storage of the item changes. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage UI")
		UPanelWidget* ContainerWidget;
};
