#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Index.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_SlotModule.h"
#include "TGOR_EuclideanModule.generated.h"

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_EuclideanModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_EuclideanModule();
};

