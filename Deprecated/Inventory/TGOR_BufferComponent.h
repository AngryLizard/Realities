// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Components/Inventory/TGOR_ContainerComponent.h"
#include "Realities/Base/Instances/Inventory/TGOR_ItemInstance.h"

#include "TGOR_BufferComponent.generated.h"

/**
* This component can be attached to every actor to give them the functionallity to temporarily store item instances.
*/
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_BufferComponent : public UTGOR_ContainerComponent
{
	GENERATED_BODY()

public:
	UTGOR_BufferComponent();
	void BeginPlay() override;

	virtual bool CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const override;
	virtual int32 GetContainerCapacity(const FTGOR_ContainerIndex& Index) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Number of items that can be queued for assembly. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		int32 BufferSize;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Puts a storage into this buffer. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PutIntoBuffer(UTGOR_ItemStorage* Storage);

	/** Checks whether there exists an empty slot to put an item into. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		bool HasEmptySlot() const;

	/** Takes a storage from this buffer. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* PickFromBuffer();

	/** Checks whether there exists a full slot to take an item from. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		bool HasFullSlot() const;

protected:

};
