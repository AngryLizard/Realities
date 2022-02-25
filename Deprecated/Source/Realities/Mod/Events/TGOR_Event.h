// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Instances/Inventory/TGOR_StorageInstance.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Event.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Event : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Event();

	/** Whether this event is relevant for given priority. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		bool IsRelevant(ETGOR_NetvarEnumeration Priority) const;

protected:

	/** Priority used for networking. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Storage")
		ETGOR_NetvarEnumeration EventPriority;
};
