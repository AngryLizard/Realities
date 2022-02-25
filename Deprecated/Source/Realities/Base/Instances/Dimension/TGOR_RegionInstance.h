// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Instances/Dimension/TGOR_ActorInstance.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_RegionInstance.generated.h"

class UTGOR_Singleton;

/**
* TGOR_RegionInstance stores region data
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_RegionInstance
{
	GENERATED_USTRUCT_BODY()
		FTGOR_RegionInstance();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTGOR_ActorState> Register;
};
