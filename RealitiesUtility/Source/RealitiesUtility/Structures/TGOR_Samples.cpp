// The Gateway of Realities: Planes of Existence.


#include "TGOR_Samples.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

FTGOR_Samples::FTGOR_Samples()
{
}

FTGOR_Samples::FTGOR_Samples(const TArray<FVector>& Data)
	: Data(Data)
{
}

FVector FTGOR_Samples::Mean() const
{
	FVector Sum = FVector::ZeroVector;
	for (const FVector& Sample : Data)
	{
		Sum += Sample;
	}
	return Sum / Data.Num();
}

float FTGOR_Samples::Radius(const FVector& Center) const
{
	float Dist = 0.0f;
	for (const FVector& Sample : Data)
	{
		Dist = FMath::Max(Dist, (Sample - Center).SizeSquared());
	}
	return FMath::Sqrt(Dist);
}

float FTGOR_Samples::RadiusAlong(const FVector& Normal) const
{
	float Max = 0.0f;
	for (const FVector& Sample : Data)
	{
		const float Dist = Sample | Normal;
		Max = FMath::Max(Max, FMath::Abs(Dist));
	}
	return Max;
}

FTGOR_Samples FTGOR_Samples::CenterData(const FVector& Center) const
{
	FTGOR_Samples Samples(Data);
	for (FVector& Sample : Samples.Data)
	{
		Sample -= Center;
	}
	return Samples;
}

FTGOR_Samples FTGOR_Samples::ProjectData(const FVector& Normal) const
{
	FTGOR_Samples Samples(Data);
	for (FVector& Sample : Samples.Data)
	{
		const float Dist = Sample | Normal;
		Sample -= Dist * Normal;
	}
	return Samples;
}

void FTGOR_Samples::Pca(int Iterations, FQuat& Quat, FVector& Extend, FVector& Center) const
{
	Center = Mean();
	FTGOR_Samples Centered = CenterData(Center);
	FTGOR_Matrix3x3 Cov = FTGOR_Matrix3x3(Centered.Data);

	const FVector Primary = Cov.PowerMethod(FVector::ForwardVector, Iterations);
	if (Primary.SizeSquared() < SMALL_NUMBER)
	{
		Quat = FQuat::Identity;
		Extend = FVector::ZeroVector;
	}
	else
	{
		Quat = FQuat::FindBetweenNormals(FVector::ForwardVector, Primary);

		Cov = FTGOR_Matrix3x3(Centered.ProjectData(Primary).Data);
		const FVector Secondary = Cov.PowerMethod(FVector::UpVector, Iterations);
		if (Secondary.SizeSquared() < SMALL_NUMBER)
		{
			Extend = FVector(Centered.RadiusAlong(Primary), 0.0f, 0.0f);
		}
		else
		{
			Quat = FQuat::FindBetweenNormals(Quat.GetAxisY(), Secondary) * Quat;

			const FVector Ternary = UTGOR_Math::Normalize(Primary ^ Secondary);
			Extend = FVector(Centered.RadiusAlong(Primary), Centered.RadiusAlong(Secondary), Centered.RadiusAlong(Ternary));
		}
	}
}
