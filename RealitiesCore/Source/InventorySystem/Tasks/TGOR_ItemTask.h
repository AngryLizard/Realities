// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "InventorySystem/TGOR_ItemInstance.h"
#include "DimensionSystem/TGOR_MobilityInstance.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "CoreSystem/Tasks/TGOR_Task.h"
#include "TGOR_ItemTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Item;
class UTGOR_ItemComponent;
class UTGOR_InventoryComponent;
class ATGOR_ItemDropActor;

/**
*
*/
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FTGOR_ItemTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Item content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_Item* Content = nullptr;

	/** Owning Item component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_ItemComponent* Component = nullptr;

	/** Slot this Item resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		int32 Slot = -1;
};

/**
*
*/
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEM_API UTGOR_ItemTask : public UTGOR_Task
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ItemTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets assigned Item content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_Item* GetItem() const;

	/** Gets assigned Item component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemComponent* GetOwnerComponent() const;

	/** Gets assigned Item slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	/** Whether this item fits a given restriction. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool IsSupportedBy(const FTGOR_ItemRestriction& Restriction) const;

	/** Create an item drop from this task, by default spawns at owning item component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		ATGOR_ItemDropActor* CreateItemDrop();

	/** Let owner know that item has been freed (usually called from inventory) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void FreeToOwner();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called once when this task is created */
	virtual bool Initialise();

	/** Called right before this task is removed from the current inventory */
	virtual void RemovedFromInventory();

	/** Called right after this task was added to the current inventory */
	virtual void AddedToInventory();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnInitialise();

	/** Called right before getting removed from an inventory (current inventory still points to the inventory we're leaving). */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnRemovedFromInventory();

	/** Called right after getting added to an inventory (current inventory points to the inventory we've entered). */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnAddedToInventory();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called when moved to a new inventory (or null if removed from one) */
	virtual void AssignInventory(UTGOR_InventoryComponent* Inventory);

	/** Whether this item is currently owned by an inventory */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual bool HasInventory() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning Item component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Inventory")
		FTGOR_ItemTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier()
	{
		Initialise();
	}

	/** To this task relevant inventory component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TWeakObjectPtr<UTGOR_InventoryComponent> InventoryComponent;

};
