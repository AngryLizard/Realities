// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ElementInstance.generated.h"

#define POS_MAX(A, B) ((A < 0.0f) ? FMath::Max(0.0f, B) : FMath::Max(A, B))
#define NEG_MIN(A, B) ((A > 0.0f) ? FMath::Min(0.0f, B) : FMath::Min(A, B))
#define BET_MIN_MAX(X, A, B) (A <= X && X <= B)

/**
* TGOR_RegionState stores a state within regions
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ElementInstance
{
	GENERATED_USTRUCT_BODY()
	FTGOR_ElementInstance();
	FTGOR_ElementInstance(float Value);
	FTGOR_ElementInstance operator/(float Factor) const;
	FTGOR_ElementInstance operator*(float Factor) const;
	FTGOR_ElementInstance operator-(const FTGOR_ElementInstance& State) const;
	FTGOR_ElementInstance operator+(const FTGOR_ElementInstance& State) const;
	FTGOR_ElementInstance PosMax(const FTGOR_ElementInstance& State) const;
	FTGOR_ElementInstance NegMin(const FTGOR_ElementInstance& State) const;
	bool IsInBetween(const FTGOR_ElementInstance& Min, const FTGOR_ElementInstance& Max) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Pressure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Partition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Moisture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dissolution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Charge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Potency;
};