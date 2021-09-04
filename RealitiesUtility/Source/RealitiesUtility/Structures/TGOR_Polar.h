// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Polar.generated.h"

/**
* 3D polar coordinates that can be packed (X: [0,1], Y: [0,1], R:[0,1])
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Polar
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Polar();
	FTGOR_Polar(float X, float Y, float R);
	bool operator==(const FTGOR_Polar& Other) const;

	uint16 Pack() const;
	void Unpack(uint16 Short);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float R;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<> struct TStructOpsTypeTraits<FTGOR_Polar> : public TStructOpsTypeTraitsBase2<FTGOR_Polar>
{
	enum { WithNetSerializer = true };
};