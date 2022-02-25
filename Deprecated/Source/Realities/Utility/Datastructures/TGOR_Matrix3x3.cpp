// The Gateway of Realities: Planes of Existence.


#include "TGOR_Matrix3x3.h"

FTGOR_Matrix3x3::FTGOR_Matrix3x3()
	: X(FVector::ZeroVector), Y(FVector::ZeroVector), Z(FVector::ZeroVector)
{
}

FTGOR_Matrix3x3::FTGOR_Matrix3x3(const FQuat& Q)
	: X(Q.GetAxisX()), Y(Q.GetAxisY()), Z(Q.GetAxisZ())
{
}

FTGOR_Matrix3x3::FTGOR_Matrix3x3(const FVector& X, const FVector& Y, const FVector& Z)
	: X(X), Y(Y), Z(Z)
{
}

FVector FTGOR_Matrix3x3::operator* (const FVector& Other) const
{
	return(FVector(Other | X, Other | Y, Other | Z));
}

FVector FTGOR_Matrix3x3::CholeskyInvert(const FVector& Input) const
{
	// Cholesky decomposition, first column
	const float lXX = FMath::Sqrt(X.X);
	if (lXX < SMALL_NUMBER) return(Input);
	const float lXY = X.Y / lXX;
	const float lXZ = X.Z / lXX;

	// Cholesky decomposition, second column. Assume Identity on failure.
	const float lYY_ = Y.Y - lXY * lXY;
	if (lYY_ < 0.0f) return(Input);
	const float lYY = FMath::Sqrt(lYY_);
	if (lYY < SMALL_NUMBER) return(Input);
	const float lYZ = (Y.Z - lXY * lXZ / lYY);

	// Cholesky decomposition, third column. Assume Identity on failure.
	const float lZZ_ = Z.Z - lXZ * lXZ - lYZ * lYZ;
	if (lZZ_ < 0.0f) return(Input);
	const float lZZ = FMath::Sqrt(lZZ_);
	if (lZZ < SMALL_NUMBER) return(Input);

	// Forward substitution
	FVector B;
	B.X = (Input.X) / lXX;
	B.Y = (Input.Y - lXY * B.X) / lYY;
	B.Z = (Input.Z - lXZ * B.X - lYZ * B.Y) / lZZ;

	// Backward substitution
	FVector R;
	R.Z = (B.Z) / lZZ;
	R.Y = (B.Y - lYZ * R.Z) / lYY;
	R.X = (B.X - lXY * R.Y - lXZ * R.Z) / lXX;

	return(R);
}