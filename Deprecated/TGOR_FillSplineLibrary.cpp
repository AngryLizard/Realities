// The Gateway of Realities: Planes of Existence.

#include "TGOR_FillSplineComponent.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"


UTGOR_FillSplineComponent::UTGOR_FillSplineComponent(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	EnableProjectUV(true),
	UnwrapLane(0.5f)
{
}

float UTGOR_FillSplineComponent::GetAveragePointDistance() const
{
	float AverageDistance = 0.0f;

	const int32 LeftNum = LeftSpline->GetNumberOfSplineSegments();
	const int32 RightNum = RightSpline->GetNumberOfSplineSegments();
	const int32 Points = FMath::Min(LeftNum, RightNum);
	for (int32 Point = 0; Point < Points; Point++)
	{
		// Compute position on the spline
		const FVector From = LeftSpline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FVector To = RightSpline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		AverageDistance += (To - From).Size();
	}
	return AverageDistance / Points;
}


float UTGOR_FillSplineComponent::GetMaxPointDistance() const
{
	float MaxDistance = 0.0f;

	const int32 LeftNum = LeftSpline->GetNumberOfSplineSegments();
	const int32 RightNum = RightSpline->GetNumberOfSplineSegments();
	const int32 Points = FMath::Min(LeftNum, RightNum);
	for (int32 Point = 0; Point < Points; Point++)
	{
		// Compute position on the spline
		const FVector From = LeftSpline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FVector To = RightSpline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		MaxDistance = FMath::Max(MaxDistance, (To - From).Size());
	}
	return MaxDistance / Points;
}

float UTGOR_FillSplineComponent::GetMinPointDistance() const
{
	float MinDistance = 0.0f;

	const int32 LeftNum = LeftSpline->GetNumberOfSplineSegments();
	const int32 RightNum = RightSpline->GetNumberOfSplineSegments();
	const int32 Points = FMath::Min(LeftNum, RightNum);
	for (int32 Point = 0; Point < Points; Point++)
	{
		// Compute position on the spline
		const FVector From = LeftSpline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FVector To = RightSpline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		MinDistance = FMath::Min(MinDistance, (To - From).Size());
	}
	return MinDistance / Points;
}

void UTGOR_FillSplineComponent::Generate(USplineComponent* Left, USplineComponent* Right)
{
	LeftSpline = Left;
	RightSpline = Right;

	GenerateMesh(PreviewLOD);
}