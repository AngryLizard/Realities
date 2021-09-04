// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Direction.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Direction
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Direction();
	FTGOR_Direction(const FVector& Vector);
	operator bool() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Magnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Direction;
};