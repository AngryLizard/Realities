// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Matrix3x3.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_Matrix3x3
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Matrix3x3();
	FTGOR_Matrix3x3(const FQuat& Q);
	FTGOR_Matrix3x3(const FVector& X, const FVector& Y, const FVector& Z);
	FVector operator* (const FVector& Other) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Z;


	FVector CholeskyInvert(const FVector& Input) const;
};