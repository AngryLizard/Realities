// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_Matrix3x3.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Matrix3x3
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Matrix3x3();
	FTGOR_Matrix3x3(const FQuat& Q); // Rotation
	FTGOR_Matrix3x3(const FVector& A, const FVector& B); // Outer procuct
	FTGOR_Matrix3x3(const TArray<FVector>& Samples); // Covariance
	FTGOR_Matrix3x3(const FVector& X, const FVector& Y, const FVector& Z); // Build
	FVector operator* (const FVector& Other) const;

	float SizeSquared() const;
	FTGOR_Matrix3x3 operator+=(const FTGOR_Matrix3x3& Other);
	FTGOR_Matrix3x3 operator+ (const FTGOR_Matrix3x3& Other) const;
	FTGOR_Matrix3x3 operator-=(const FTGOR_Matrix3x3& Other);
	FTGOR_Matrix3x3 operator- (const FTGOR_Matrix3x3& Other) const;
	FTGOR_Matrix3x3 operator* (float Scale) const;

	float& operator()(int32 I, int32 J);
	float operator()(int32 I, int32 J)const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Z;


	FVector Diag() const;
	FVector PowerMethod(const FVector& Input, int32 Iterations) const;
	FVector CholeskyInvert(const FVector& Input) const;
	float Det() const;

	static const FTGOR_Matrix3x3 Identity;
};