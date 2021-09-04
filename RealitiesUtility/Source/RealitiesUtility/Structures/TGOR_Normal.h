// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Normal.generated.h"

/**
* A floating point value between 0 and 1
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Normal
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Normal();
	FTGOR_Normal(float V);
	bool operator==(const FTGOR_Normal& Normal) const;

	uint8 Pack() const;
	void Unpack(uint8 Byte);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Value;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<> struct TStructOpsTypeTraits<FTGOR_Normal> : public TStructOpsTypeTraitsBase2<FTGOR_Normal>
{
	enum { WithNetSerializer = true };
};