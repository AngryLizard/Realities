// The Gateway of Realities: Planes of Existence.


#include "TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

const FTGOR_Matrix3x3 FTGOR_Matrix3x3::Identity(FVector::ForwardVector, FVector::RightVector, FVector::UpVector);

FTGOR_Matrix3x3::FTGOR_Matrix3x3()
	: X(FVector::ZeroVector), Y(FVector::ZeroVector), Z(FVector::ZeroVector)
{
}

FTGOR_Matrix3x3::FTGOR_Matrix3x3(const FQuat& Q)
	: X(Q.GetAxisX()), Y(Q.GetAxisY()), Z(Q.GetAxisZ())
{
}

FTGOR_Matrix3x3::FTGOR_Matrix3x3(const FVector& A, const FVector& B)
	: FTGOR_Matrix3x3()
{
	for (int32 I = 0; I < 3; I++)
	{
		for (int32 J = 0; J < 3; J++)
		{
			(*this)(I, J) += A[I] * B[J];
		}
	}
}

FTGOR_Matrix3x3::FTGOR_Matrix3x3(const TArray<FVector>& Samples)
	: FTGOR_Matrix3x3()
{
	const int32 N = Samples.Num();

	const double InvN = 1.0 / (N - 1);
	for (int32 I = 0; I < 3; I++)
	{
		for (int32 J = 0; J < 3; J++)
		{
			for (const FVector& Sample : Samples)
			{
				(*this)(I, J) += (Sample[I]) * (Sample[J]) * InvN;
			}

		}
	}
}

FTGOR_Matrix3x3::FTGOR_Matrix3x3(const FVector& X, const FVector& Y, const FVector& Z)
	: X(X), Y(Y), Z(Z)
{
}

FVector FTGOR_Matrix3x3::operator* (const FVector& Other) const
{
	return(FVector(Other | X, Other | Y, Other | Z));
}

double FTGOR_Matrix3x3::SizeSquared() const
{
	return X.SizeSquared() + Y.SizeSquared() + Z.SizeSquared();
}

FTGOR_Matrix3x3 FTGOR_Matrix3x3::operator+=(const FTGOR_Matrix3x3& Other)
{
	X += Other.X;
	Y += Other.Y;
	Z += Other.Z;
	return *this;
}

FTGOR_Matrix3x3 FTGOR_Matrix3x3::operator+ (const FTGOR_Matrix3x3& Other) const
{
	FTGOR_Matrix3x3 Out;
	Out.X = X + Other.X;
	Out.Y = Y + Other.Y;
	Out.Z = Z + Other.Z;
	return Out;
}

FTGOR_Matrix3x3 FTGOR_Matrix3x3::operator-=(const FTGOR_Matrix3x3& Other)
{
	X -= Other.X;
	Y -= Other.Y;
	Z -= Other.Z;
	return *this;
}

FTGOR_Matrix3x3 FTGOR_Matrix3x3::operator- (const FTGOR_Matrix3x3& Other) const
{
	FTGOR_Matrix3x3 Out;
	Out.X = X - Other.X;
	Out.Y = Y - Other.Y;
	Out.Z = Z - Other.Z;
	return Out;
}

FTGOR_Matrix3x3 FTGOR_Matrix3x3::operator* (double Scale) const
{
	FTGOR_Matrix3x3 Out;
	Out.X = X * Scale;
	Out.Y = Y * Scale;
	Out.Z = Z * Scale;
	return Out;
}

double& FTGOR_Matrix3x3::operator()(int32 I, int32 J)
{
	check(I >= 0 && I < 3 && J >= 0 && J < 3);
	return (&X.X)[I * 3 + J];
}

double FTGOR_Matrix3x3::operator()(int32 I, int32 J) const
{
	check(I >= 0 && I < 3 && J >= 0 && J < 3);
	return (&X.X)[I * 3 + J];
}


FVector FTGOR_Matrix3x3::Diag() const
{
	return FVector(X.X, Y.Y, Z.Z);
}

FVector FTGOR_Matrix3x3::PowerMethod(const FVector& Input, int32 Iterations) const
{
	FVector Out = Input;
	for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
	{
		const FVector B = (*this) * Out;
		if (B.SizeSquared() < SMALL_NUMBER)
		{
			Out = UTGOR_Math::Normalize(Out + FMath::VRand() * 0.05);
		}
		else
		{
			Out = UTGOR_Math::Normalize(B);
		}
	}
	return Out;
}

FVector FTGOR_Matrix3x3::CholeskyInvert(const FVector& Input) const
{
	// Cholesky decomposition, first column
	const double lXX = FMath::Sqrt(X.X);
	if (lXX < SMALL_NUMBER) return(Input);
	const double lXY = X.Y / lXX;
	const double lXZ = X.Z / lXX;

	// Cholesky decomposition, second column. Assume Identity on failure.
	const double lYY_ = Y.Y - lXY * lXY;
	if (lYY_ < 0.0) return(Input);
	const double lYY = FMath::Sqrt(lYY_);
	if (lYY < SMALL_NUMBER) return(Input);
	const double lYZ = (Y.Z - lXY * lXZ / lYY);

	// Cholesky decomposition, third column. Assume Identity on failure.
	const double lZZ_ = Z.Z - lXZ * lXZ - lYZ * lYZ;
	if (lZZ_ < 0.0) return(Input);
	const double lZZ = FMath::Sqrt(lZZ_);
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

double FTGOR_Matrix3x3::Det() const
{
	return 
		X.X * Y.Y * Z.Z +
		X.Y * Y.Z * Z.X +
		X.Z * Y.X * Z.Y -
		Z.X * Y.Y * X.Z -
		Z.Y * Y.Z * X.X -
		Z.Z * Y.X * X.Y;
}
