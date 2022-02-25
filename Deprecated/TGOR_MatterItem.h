#pragma once

#include "Realities/Mod/Energies/TGOR_Matter.h"

#include "TGOR_Item.h"
#include "TGOR_MatterItem.generated.h"

/**
* The base class for all items that have matter storage.
*/
UCLASS(Blueprintable, BlueprintType)
class REALITIES_API UTGOR_MatterItem : public UTGOR_Item
{
	GENERATED_BODY()

	friend class UTGOR_MatterStorage;

public:
	UTGOR_MatterItem();
	virtual void BuildStorage(UTGOR_ItemStorage* Storage) override;

protected:

	/** Matter quantity per matter type this item is able to store. */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		TMap<TSubclassOf<UTGOR_Matter>, int32> MatterStorage;

};
