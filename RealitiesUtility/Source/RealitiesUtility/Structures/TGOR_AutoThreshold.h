// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_AutoThreshold.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_AutoThreshold
{
	GENERATED_USTRUCT_BODY()

	FTGOR_AutoThreshold();
	FTGOR_AutoThreshold(float InitialThreshold);
	bool Update(float Deltatime, float Error);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float UpdateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Multiplier;

	UPROPERTY(BlueprintReadWrite)
		float Timer;

	UPROPERTY(BlueprintReadWrite)
		float Average;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxThreshold;
};