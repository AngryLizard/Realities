// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance_OLD.h"
#include "Base/Instances/Inventory/TGOR_InventoryInstance.h"
#include "Base/Instances/System/TGOR_ActionInstance.h"

#include "UI/Queries/TGOR_SelectionQuery.h"
#include "TGOR_InventoryQuery.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_PlayerController;
class UTGOR_ContainerComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_InventoryQuery : public UTGOR_SelectionQuery
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_InventoryQuery();

	virtual TArray<FTGOR_Display> QueryDisplays() const override;
	virtual void CallIndex(int32 Index) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintPure, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		float GetHotkeyQuantity(const FTGOR_SlotIndex& Index) const;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void DropIndex(const FTGOR_SlotIndex& Index, const FVector& Location);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void SwapIndex(const FTGOR_SlotIndex& Index, UTGOR_InventoryQuery* Target, const FTGOR_SlotIndex& TargetIndex);

	/** Fills this constructor with all item slots given a subslot */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void Assign(ATGOR_PlayerController* Owner, UTGOR_ContainerComponent* Container, const FTGOR_SlotIndex& Index);
	
	/** Controller that manages this inventory */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Inventory")
		ATGOR_PlayerController* ContainerOwner;

	/** Inventory the slots render from */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Inventory")
		UTGOR_ContainerComponent* SourceContainer;

	/** Inventory subslot the slots render from */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Inventory")
		FTGOR_SlotIndex SourceIndex;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
};
