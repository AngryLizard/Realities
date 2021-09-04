// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"

#include "TGOR_GMM.generated.h"

// TODO: Not validated to be working yet

USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Distribution
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Distribution();
	FTGOR_Distribution(const TArray<FVector>& Samples);
	float Pdf(const FVector& X) const;

	/** Covariance matrix */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FTGOR_Matrix3x3 Cov;

	/** Mean position */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector Mu;

	/** Weight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Pi;
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_GMM
{
	GENERATED_USTRUCT_BODY()

		FTGOR_GMM();

	float Step();
	void Simplify(float Threshold);
	void AddPoint(const FVector& Point);

	void EM(int32 MaxIterations, float Threshold);

	/** Points */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FVector> Points;

	/** Distributions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FTGOR_Distribution> Distributions;
};