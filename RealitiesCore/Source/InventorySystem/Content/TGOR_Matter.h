// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Content/TGOR_Energy.h"
#include "TGOR_Matter.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class INVENTORYSYSTEM_API UTGOR_Matter : public UTGOR_Energy
{
	GENERATED_BODY()

public:
	UTGOR_Matter();

	/** Mass of one matter unit in kg */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Matter")
		float Mass;
};