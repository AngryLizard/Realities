// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_PackageMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Buffer.generated.h"

/**
* FTGOR_Buffer wraps byte arrays
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_Buffer
{
	GENERATED_USTRUCT_BODY()
		FTGOR_Buffer();

	UPROPERTY(BlueprintReadWrite)
		TArray<uint8> Data;

	void Clear();
	bool HasData() const;

	bool operator!=(const FTGOR_Buffer& Buffer) const;
	bool operator==(const FTGOR_Buffer& Buffer) const;
};

