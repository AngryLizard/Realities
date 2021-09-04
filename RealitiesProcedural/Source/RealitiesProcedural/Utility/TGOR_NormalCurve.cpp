// The Gateway of Realities: Planes of Existence.


#include "TGOR_NormalCurve.h"


FTGOR_NormalCurve::FTGOR_NormalCurve()
:	X(FVector::ZeroVector), Y(FVector::ZeroVector)
{
}

bool FTGOR_NormalCurve::operator==(const FTGOR_NormalCurve& Other) const
{
	return X == Other.X && Y == Other.Y;
}

FVector2D FTGOR_NormalCurve::Sample(float Time) const
{
	const FVector2D A = FMath::Lerp(FVector2D(X.X, Y.X), FVector2D(X.Y, Y.Y), Time);
	const FVector2D B = FMath::Lerp(FVector2D(X.Y, Y.Y), FVector2D(X.Z, Y.Z), Time);
	return FMath::Lerp(A, B, Time);
}


FTGOR_UnitCurve::FTGOR_UnitCurve()
{
	CurveData.AddKey(0.00f, 0.0f);
	CurveData.AddKey(0.25f, 0.0f);
	CurveData.AddKey(0.50f, 0.0f);
	CurveData.AddKey(0.75f, 0.0f);
	CurveData.AddKey(1.00f, 0.0f);
}

bool FTGOR_UnitCurve::operator==(const FTGOR_UnitCurve& Other) const
{
	return CurveData == Other.CurveData;
}

float FTGOR_UnitCurve::Sample(float Time) const
{
	return CurveData.Eval(Time, 0.0f);
}