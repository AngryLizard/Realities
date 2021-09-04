// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_ItemInstance.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_ItemQuery.generated.h"

class UTGOR_Item;
class UTGOR_InventoryComponent;

/**
 * UTGOR_InventoryQuery
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_ItemQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ItemQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Constructs inventory from owning inventory component, filtered by given action index and optional filter. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_InventoryComponent* InventoryComponent, const FTGOR_ItemRestriction& Filter);

	/** Custom item filter function */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual bool FilterItem(UTGOR_Item* Item) const;

	/** Refresh list from inventory. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void RefreshItems();

	/** Inventory component assigned to this query */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TWeakObjectPtr<UTGOR_InventoryComponent> OwnerComponent;

	/** Item slots */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<int32> SlotIdentifiers;

	/** Filter applied to all items */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Inventory")
		FTGOR_ItemRestriction SlotFilter;
};
