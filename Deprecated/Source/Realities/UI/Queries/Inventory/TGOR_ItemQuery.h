// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Inventory/TGOR_ItemInstance.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_ItemQuery.generated.h"

class UTGOR_Item;
class UTGOR_ActionComponent;

/**
 * UTGOR_InventoryQuery
 */
UCLASS()
class REALITIES_API UTGOR_ItemQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ItemQuery();
	virtual TArray<UTGOR_Content*> QueryContent() const override;

	/** Constructs inventory from owning inventory component, filtered by given action index and optional filter. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void AssignComponent(UTGOR_ActionComponent* ActionComponent, const FTGOR_SlotIndex& Index, const FTGOR_ItemRestriction& Filter);

	/** Custom item filter function */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual bool FilterItem(UTGOR_ItemStorage* Storage) const;

	/** Only valid if constructed from AssignComponent */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TWeakObjectPtr<UTGOR_ActionComponent> OwnerComponent;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<FTGOR_SlotIndex> Slots;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<UTGOR_Item*> Items;
};
