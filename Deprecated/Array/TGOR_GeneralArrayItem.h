#pragma once

#include "TGOR_ArrayItem.h"
#include "TGOR_GeneralArrayItem.generated.h"

class UTGOR_GeneralArrayStorage;

/**
* 
*/
UCLASS(Blueprintable, BlueprintType)
class REALITIES_API UTGOR_GeneralArrayItem : public UTGOR_ArrayItem
{
	GENERATED_BODY()

public:
	UTGOR_GeneralArrayItem();
	virtual UTGOR_ArrayStorage* CreateArrayStorage() override;
	virtual bool AcceptsItem(UTGOR_Item* Content) const override;

	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual UTGOR_GeneralArrayStorage* CreateGeneralArrayStorage();

protected:

	/** The type of storage this item has. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		TSubclassOf<UTGOR_GeneralArrayStorage> StorageType;

	/** The type of item that can be stored inside the inventory. */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		TSubclassOf<UTGOR_Item> AcceptingItem;
};
