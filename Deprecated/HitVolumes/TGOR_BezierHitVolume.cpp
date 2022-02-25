// The Gateway of Realities: Planes of Existence.


#include "TGOR_BezierHitVolume.h"




ATGOR_BezierHitVolume::ATGOR_BezierHitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OffsetDirection = FVector(0.0f, 0.0f, 1.0f);
	OffsetVariance = FVector(0.0f, 1.0f, 0.0f);
	OffsetTarget = FVector(0.0f, 0.0f, 0.0f);
}


FVector ATGOR_BezierHitVolume::RandomVector(const FVector& Range)
{
	float X = (FMath::SRand() * 2 - 1.0f) * Range.X;
	float Y = (FMath::SRand() * 2 - 1.0f) * Range.Y;
	float Z = (FMath::SRand() * 2 - 1.0f) * Range.Z;
	return(FVector(X, Y, Z));
}

void ATGOR_BezierHitVolume::Setup(FTGOR_FunctionSegment& Segment)
{
	// Compute random offset
	FMath::SRandInit(Segment.Index + Identifier);
	FVector Offset = RandomVector(OffsetVariance);
	FVector Target = RandomVector(OffsetTarget);
	
	// Get bezier curve parameters
	FVector Direction = OffsetDirection + Offset;
	Segment.Primary = Direction * Segment.Distance / 2;
	Segment.Secondary = FVector(Segment.Distance, 0.0f, 0.0f) + Target * Segment.Distance;
}

FVector ATGOR_BezierHitVolume::Function(float X, const FTGOR_FunctionSegment& Segment, float Time)
{
	if (Lifetime < SMALL_NUMBER)
	{
		return(FVector(0.0f, 0.0f, 0.0f));
	}
	
	// Get bezier parameters
	FVector Start = FVector(0.0f, 0.0f, 0.0f);
	FVector Control = Segment.Primary;
	FVector End = Segment.Secondary;

	// Get position on the curve
	float T = X / Lifetime;
	float NT = (1.0f - T);

	FVector Bezier = NT * ( NT * Start + 2 * T * Control) + T * T * End;

	return(Bezier);
}