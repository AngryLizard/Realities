#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "TGOR_ItemInstance.generated.h"

class UTGOR_Item;
class UTGOR_ItemTask;

/**
*
*/
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FTGOR_ItemRestriction
{
	GENERATED_USTRUCT_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Inventory")
		TSubclassOf<UTGOR_Item> Item;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Inventory")
		TSubclassOf<UTGOR_ItemTask> Task;
};

