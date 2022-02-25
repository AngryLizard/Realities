#pragma once

#include "CoreMinimal.h"
#include "UI/TGOR_Widget.h"
#include "UI/Inventory/TGOR_StorageContainerMenu.h"

#include "TGOR_StorageSlotWidget.generated.h"

/**
 * The base class for all item storage slots.
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_StorageSlotWidget : public UTGOR_Widget
{
	GENERATED_BODY()

	friend class UTGOR_StorageContainerMenu;

public:
	UTGOR_StorageSlotWidget();

	/** Initialize the slot. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual void InitializeSlot(UTGOR_StorageContainerMenu* NewParentContainer, int32 NewIndex);

	/** Swaps the contens of two slots. The slots can be in different storages. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		virtual bool SwapContents(UTGOR_StorageSlotWidget* Other);

	/** Return the index of the slot in the parent container. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		int32 GetIndex() const;

	// TODO: const on parent container?
	/** Return the parent container of this slot. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage UI", Meta = (Keywords = "C++"))
		UTGOR_StorageContainerMenu* GetParentContainer();

	/** Wrapper for getting the casted item storage. */
	template<typename T>
	T* GetStorageCasted() const
	{
		if (IsValid(ParentContainer))
		{
			return Cast<T>(ParentContainer->GetStorage());			
		}

		return nullptr;
	}

protected:
	/** The index of this slot in the parent container. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage UI")
		int32 Index;

	/** The parent container of this slot, holding this slot. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage UI")
		UTGOR_StorageContainerMenu* ParentContainer;
};
