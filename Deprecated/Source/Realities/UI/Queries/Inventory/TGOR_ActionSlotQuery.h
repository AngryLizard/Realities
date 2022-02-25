// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Mod/Actions/Equipables/Inventories/TGOR_InventoryAction.h"

#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_ActionSlotQuery.generated.h"

class UTGOR_Item;
class UTGOR_ActionComponent;
class UTGOR_InventoryStorage;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ActionSlotQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ActionSlotQuery();
	virtual TArray<UTGOR_Content*> QueryContent() const override;

	/** Assigns ActionComponent and last used inventory slot, also defines an additional filter used for SlotActions */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_Action> Type, bool CheckCanCall = false);

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_ActionComponent* OwnerComponent;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<FTGOR_SlotIndex> Slots;

private:
};
