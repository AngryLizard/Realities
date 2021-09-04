// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Contacts.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_FlatContact
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_FlatContact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Blocking;
};
