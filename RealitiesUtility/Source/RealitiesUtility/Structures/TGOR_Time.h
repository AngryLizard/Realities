// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Time.generated.h"

#define SECONDS_PER_EPOCH 86'400.0f

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Time
{
	GENERATED_USTRUCT_BODY()
	FTGOR_Time();

	void Normalize();
	void Clear(); // Set to 0
	bool IsZero() const; // Whether it is 0

	float Modf(float Period = (2*PI), float Speed = 1.0f, float Phase = 0.0f) const;
	float Value() const; // Incremental value for visuals, jumps every epoch
	FString Print() const;

	bool operator==(const FTGOR_Time& Other) const;
	bool operator!=(const FTGOR_Time& Other) const;
	bool operator>(const FTGOR_Time& Other) const;
	bool operator<(const FTGOR_Time& Other) const;
	float operator-(const FTGOR_Time& Other) const;
	FTGOR_Time operator+=(float DeltaSeconds);
	FTGOR_Time operator+(float DeltaSeconds) const;

	UPROPERTY()
		float Seconds;

	UPROPERTY()
		uint16 Epochs;


	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<> struct TStructOpsTypeTraits<FTGOR_Time> : public TStructOpsTypeTraitsBase2<FTGOR_Time>
{
	enum { WithNetSerializer = true };
};