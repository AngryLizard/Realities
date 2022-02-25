// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Interfaces/TGOR_SaveInterface.h"
#include "TGOR_ContainerComponent.h"
#include "TGOR_SingleItemComponent.generated.h"

/**
* UTGOR_SingleItemComponent adds an inventory to an actor that can hold one item
*/
UCLASS()
class REALITIES_API UTGOR_SingleItemComponent : public UTGOR_ContainerComponent, public ITGOR_SaveInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_SingleItemComponent();
	virtual void InventoryUpdate() override;
	virtual void InventoryRebuild() override;
	virtual void ForEachSlot(std::function<bool(const FTGOR_SlotIndex& Index, FTGOR_ItemInstance_OLD& ItemInstance)> func) override;
	virtual FTGOR_ItemInstance_OLD& GetSlot(const FTGOR_SlotIndex& Index, ETGOR_FetchEnumeration& Branches) override;

	virtual void Load_Implementation(const FTGOR_Buffer& Buffer) override;
	virtual bool Store_Implementation(FTGOR_Buffer& Buffer) override;

	// Replicate
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "TGOR|Inventory")
		FTGOR_ItemInstance_OLD ItemInstance;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fill item data */
	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, Category = "TGOR|Inventory", Meta = (Keywords = "C++"))
		void AssignItem(const FTGOR_ItemInstance_OLD& Instance);
		virtual void AssignItem_Implementation(const FTGOR_ItemInstance_OLD& Instance);
};
