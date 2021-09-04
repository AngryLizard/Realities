// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/TGOR_ActorInstance.h"

#include "TGOR_RegionInstance.generated.h"

class UTGOR_Singleton;

/**
* TGOR_RegionInstance stores region data
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_RegionInstance
{
	GENERATED_USTRUCT_BODY()
		FTGOR_RegionInstance();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTGOR_ActorState> Register;
};
