// The Gateway of Realities: Planes of Existence.


#include "TGOR_GMM.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"


// (2 pi) ^ 3
#define TWOPI_P_3 248.0502134424f

FTGOR_Distribution::FTGOR_Distribution()
: Mu(FVector::ZeroVector), Pi(1.0f)
{
}

FTGOR_Distribution::FTGOR_Distribution(const TArray<FVector>& Samples)
	: FTGOR_Distribution()
{
	const int32 N = Samples.Num();

	Mu = FVector::ZeroVector;
	for (const FVector& Sample : Samples)
	{
		Mu += Sample;
	}
	Mu /= N;

	const float InvN = 1.0f / (N - 1);
	for (int32 I = 0; I < 3; I++)
	{
		for (int32 J = 0; J < 3; J++)
		{
			for (const FVector& Sample : Samples)
			{
				Cov(I, J) += (Sample[I] - Mu[I]) * (Sample[J] - Mu[J]) * InvN;
			}
		}
	}
}

float FTGOR_Distribution::Pdf(const FVector& X) const
{
	const FVector Delta = X - Mu;
	const float Exp = FMath::Exp(-0.5f * (Delta | Cov.CholeskyInvert(Delta)));
	const float Det = TWOPI_P_3 * Cov.Det();
	if (Det < SMALL_NUMBER)
	{
		return 0.0f;
	}
	return (Pi * Exp) / Det;
}

FTGOR_GMM::FTGOR_GMM()
{

}

float FTGOR_GMM::Step()
{
	const int32 DNum = Distributions.Num();
	const int32 PNum = Points.Num();
	if (PNum == 0) return 0.0f;

	TArray<TArray<float>> Rs;

	// EStep
	Rs.SetNumZeroed(DNum);
	for (int32 Di = 0; Di < DNum; Di++)
	{
		const FTGOR_Distribution& Distribution = Distributions[Di];

		// Compute pdfs
		float PSum = 0.0f;
		Rs[Di].SetNumUninitialized(PNum);
		for (int32 Pi = 0; Pi < PNum; Pi++)
		{
			Rs[Di][Pi] = Distribution.Pdf(Points[Pi]) * Distribution.Pi;
			PSum += Rs[Di][Pi];
		}

		// Normalise
		if (PSum > SMALL_NUMBER)
		{
			for (int32 Pi = 0; Pi < PNum; Pi++)
			{
				Rs[Di][Pi] /= PSum;
			}
		}
	}

	TArray<float> Rns;

	// Normalise
	float DSum = 0.0f;
	Rns.SetNumZeroed(DNum);
	for (int32 Di = 0; Di < DNum; Di++)
	{
		// Compute Pi
		for (int32 Pi = 0; Pi < PNum; Pi++)
		{
			Rns[Di] += Rs[Di][Pi];
		}
		DSum += Rns[Di];
	}

	float Change = 0.0f;

	// MStep
	if (DSum > SMALL_NUMBER)
	{
		for (int32 Di = 0; Di < DNum; Di++)
		{
			FTGOR_Distribution& Distribution = Distributions[Di];

			// Compute Pi
			Distribution.Pi = Rns[Di] / DSum;

			// Compute Mu
			const FVector OldMu = Distribution.Mu;
			Distribution.Mu = FVector::ZeroVector;
			for (int32 Pi = 0; Pi < PNum; Pi++)
			{
				Distribution.Mu += Points[Pi] * (Rs[Di][Pi] / Rns[Di]);
			}

			Change += (OldMu - Distribution.Mu).SizeSquared();

			// Compute cov
			const FTGOR_Matrix3x3 OldCov = Distribution.Cov;
			Distribution.Cov = FTGOR_Matrix3x3();
			for (int32 Pi = 0; Pi < PNum; Pi++)
			{
				const FVector Delta = Points[Pi] - Distribution.Mu;
				Distribution.Cov += FTGOR_Matrix3x3(Delta, Delta) * (Rs[Di][Pi] / Rns[Di]);
			}

			// Regularisation term
			Distribution.Cov += FTGOR_Matrix3x3::Identity;
			Change += (OldCov - Distribution.Cov).SizeSquared();
		}
	}
	return Change;
}

void FTGOR_GMM::Simplify(float Threshold)
{
	for (auto Its = Distributions.CreateIterator(); Its; ++Its)
	{
		for (auto Ots = Its + 1; Ots; ++Ots)
		{
			const float Energy = (Its->Mu - Ots->Mu).SizeSquared() + (Its->Cov - Ots->Cov).SizeSquared();
			if (Energy < Threshold * Threshold)
			{
				Its.RemoveCurrent();
				break;
			}
		}
	}
}

void FTGOR_GMM::AddPoint(const FVector& Point)
{
	Points.Emplace(Point);

	FTGOR_Distribution Distribution;
	Distribution.Cov = FTGOR_Matrix3x3::Identity;
	Distribution.Mu = Point;
	Distribution.Pi = 1.0f;
	Distributions.Emplace(Distribution);
}


void FTGOR_GMM::EM(int32 MaxIterations, float Threshold)
{
	for (FTGOR_Distribution& Distribution : Distributions)
	{
		for (int32 Iteration = 0; Iteration < MaxIterations; Iteration++)
		{
			if (Step() < KINDA_SMALL_NUMBER)
			{
				break;
			}
		}
		Simplify(Threshold);
	}
}
