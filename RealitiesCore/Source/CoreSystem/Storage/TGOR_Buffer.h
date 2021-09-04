// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Storage/TGOR_PackageMinimal.h"

#include "TGOR_Buffer.generated.h"

/**
* FTGOR_Buffer wraps byte arrays
*/
USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_Buffer
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

