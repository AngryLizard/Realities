// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Matter.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_Matter : public UTGOR_Content
{
	GENERATED_BODY()

public:
	UTGOR_Matter();

	/** Mass of one matter unit in kg */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Matter")
		float Mass;
};