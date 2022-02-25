// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UI/Inventory/TGOR_ItemSlotWidget.h"

#include "UI/TGOR_Icon.h"
#include "TGOR_ActionSlotWidget.generated.h"

class UTGOR_ActionSlotQuery;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ActionSlotWidget : public UTGOR_ItemSlotWidget
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ActionSlotWidget();
	virtual UTGOR_Storage* GetItem() const override;

	virtual void SelectItem() override;
	virtual void CommitItem() override;

	/** Reset slot with item */
	UFUNCTION(BlueprintImplementableEvent)
		void VisualiseItem(UTGOR_Item* Item);

	/** Reset slot with action */
	UFUNCTION(BlueprintImplementableEvent)
		void VisualiseAction(UTGOR_Action* Action);

	/** Displays given display */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void Assign(UTGOR_ActionSlotQuery* Query, const FTGOR_SlotIndex& Item);


	UPROPERTY(BlueprintReadOnly, Category = "TGOR Creature")
		UTGOR_ActionSlotQuery* ActionSlotQuery;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Creature")
		FTGOR_SlotIndex SlotIndex;

protected:
private:

};
