// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Percentage.generated.h"

/**
* A high precision floating point value between 0 and 1
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Percentage
{
	GENERATED_USTRUCT_BODY()

		FTGOR_Percentage();
	FTGOR_Percentage(float V);
	bool operator==(const FTGOR_Percentage& Normal) const;

	uint16 Pack() const;
	void Unpack(uint16 Byte);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Value;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<> struct TStructOpsTypeTraits<FTGOR_Percentage> : public TStructOpsTypeTraitsBase2<FTGOR_Percentage>
{
	enum { WithNetSerializer = true };
};