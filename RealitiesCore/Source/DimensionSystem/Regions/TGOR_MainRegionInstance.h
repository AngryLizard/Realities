// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_RegionInstance.h"

#include "TGOR_MainRegionInstance.generated.h"

class ATGOR_RegionActor;


/**
* TGOR_DimensionInstance stores regions
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_MainRegionInstance
{
	GENERATED_USTRUCT_BODY()
		FTGOR_MainRegionInstance();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditDefaultsOnly)
		TArray<ATGOR_RegionActor*> Regions;

};

