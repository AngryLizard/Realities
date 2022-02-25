// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Inventory/TGOR_ItemInstance.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Queries/Inventory/TGOR_ItemQuery.h"
#include "TGOR_TabQuery.generated.h"

class UTGOR_Item;
class UTGOR_ActionComponent;
class UTGOR_InventoryTabMenu;

/**
 * UTGOR_TabQuery
 */
UCLASS()
class REALITIES_API UTGOR_TabQuery : public UTGOR_ItemQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_TabQuery();
	virtual bool FilterItem(UTGOR_ItemStorage* Storage) const override;

	/** Constructs tab from owning action component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignTab(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_InventoryTabMenu> TabMenu);

	/** Reconstructs item list */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void Refresh(UTGOR_ActionComponent* ActionComponent, const FTGOR_SlotIndex& Index);

	/** Additional filter on items to be displayed for the user */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Inventory")
		FTGOR_ItemRestriction SupportedItem;

	/** Tab menu opened through this query */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Inventory")
		TSubclassOf<UTGOR_InventoryTabMenu> Tab;

};
