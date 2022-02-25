// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_ItemInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"
#include "TGOR_ContainerComponent.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_ItemDropActor;


///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemMoveDelegate, UTGOR_ItemStorage*, Item);

/**
* TGOR_ContainerComponent adds any kind of item container to an actor (abstract)
*/
UCLASS()
class INVENTORYSYSTEM_API UTGOR_ContainerComponent : public UTGOR_DimensionComponent, public ITGOR_RegisterInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_ContainerComponent();

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FSlotUpdateDelegate OnSlotUpdate;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FItemMoveDelegate OnItemDestruction;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FItemMoveDelegate OnItemDrop;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called when items are changed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory|Internal", Meta = (Keywords = "C++"))
		void ContainerUpdate();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Item drop impulse in owner local space. */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float ItemDropImpulse;

	/** Item drop impulse upwards ratio in owner local space. */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float ItemDropUpwards;

	/** How much items scatter (0 for straight forwards, 1 for 90° scatter). */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float ItemDropScatter;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Drops item to ground */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		ATGOR_ItemDropActor* DropItemStorage(UTGOR_ItemStorage* Storage);

	/** Destroys an item */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void DestroyItemStorage(UTGOR_ItemStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Removes an item from this container, returns true if at least one item was removed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory|Internal", Meta = (Keywords = "C++"))
		virtual bool PurgeItemStorage(UTGOR_ItemStorage* Storage);

protected:
private:
};
