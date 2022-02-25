// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UI/Inventory/TGOR_ItemSlotWidget.h"
#include "TGOR_InventorySlotWidget.generated.h"

class UTGOR_ItemQuery;
class UTGOR_InventoryStorage;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_InventorySlotWidget : public UTGOR_ItemSlotWidget
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_InventorySlotWidget();

	virtual void SelectItem() override;
	virtual void CommitItem() override;


	/** Reset slot with item */
	UFUNCTION(BlueprintImplementableEvent)
		void VisualiseItem(UTGOR_Item* Item);

	/** Displays given display */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void Assign(UTGOR_ItemQuery* Query, int32 Item);


	UPROPERTY(BlueprintReadOnly, Category = "TGOR Creature")
		UTGOR_ItemQuery* ItemQuery;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Creature")
		int32 ItemIndex;

protected:
private:

};
