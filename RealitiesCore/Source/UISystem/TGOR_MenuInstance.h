// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_MenuInstance.generated.h"

/**
* 
*/
USTRUCT(BlueprintType)
struct FTGOR_MenuVarParam
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_MenuVarParam();
	FTGOR_MenuVarParam(FName Name);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;
};
