// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UObject/Object.h"
#include "TGOR_TextAreaStruct.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_TextAreaStruct
{
	GENERATED_USTRUCT_BODY()

		FTGOR_TextAreaStruct();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Text;
};