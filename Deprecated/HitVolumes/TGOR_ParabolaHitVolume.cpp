// The Gateway of Realities: Planes of Existence.


#include "TGOR_ParabolaHitVolume.h"




ATGOR_ParabolaHitVolume::ATGOR_ParabolaHitVolume(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	DropOffTime = 0.0f;

	DownAcceleration = 981.0f;
	StartVelocity = 0.0f;
}



void ATGOR_ParabolaHitVolume::Setup(FTGOR_FunctionSegment& Segment)
{
	Segment.Primary = Segment.Transform.InverseTransformVectorNoScale(FVector::UpVector);
}

FVector ATGOR_ParabolaHitVolume::Function(float X, const FTGOR_FunctionSegment& Segment, float Time)
{
	const FVector Linear = FVector(X * StartVelocity, 0.0f, 0.0f);
	if (X < DropOffTime)
	{
		return(Linear);
	}
	else
	{
		const float Delta = X - DropOffTime;
		const float Value = Parabola(DownAcceleration / 2, Delta);
		return(Linear - Segment.Primary * Value);
	}
}


float ATGOR_ParabolaHitVolume::Parabola(float Strength, float X)
{
	// Quadratic formula
	return(X * X * Strength);
}
