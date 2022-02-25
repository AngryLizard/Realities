// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Base/Instances/Inventory/TGOR_ItemInstance.h"

#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Components/Dimension/TGOR_DimensionComponent.h"
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
class REALITIES_API UTGOR_ContainerComponent : public UTGOR_DimensionComponent, public ITGOR_RegisterInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_ContainerComponent();

	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Storage")
		FSlotUpdateDelegate OnSlotUpdate;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Storage")
		FItemMoveDelegate OnItemDestruction;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Storage")
		FItemMoveDelegate OnItemDrop;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called when items are changed */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void ContainerUpdate();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Item drop impulse in owner local space. */
	UPROPERTY(EditAnywhere, Category = "TGOR Storage")
		float ItemDropImpulse;

	/** Item drop impulse upwards ratio in owner local space. */
	UPROPERTY(EditAnywhere, Category = "TGOR Storage")
		float ItemDropUpwards;

	/** How much items scatter (0 for straight forwards, 1 for 90° scatter). */
	UPROPERTY(EditAnywhere, Category = "TGOR Storage")
		float ItemDropScatter;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Drops item to ground */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		ATGOR_ItemDropActor* DropItemStorage(UTGOR_ItemStorage* Storage);

	/** Destroys an item */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		void DestroyItemStorage(UTGOR_ItemStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Removes an item from this container, returns true if at least one item was removed */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool PurgeItemStorage(UTGOR_ItemStorage* Storage);

protected:
private:
};
