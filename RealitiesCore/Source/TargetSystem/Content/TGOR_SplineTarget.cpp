// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_SplineTarget.h"
#include "Engine/Classes/Components/SplineComponent.h"

#include "../Components/TGOR_AimTargetComponent.h"

UTGOR_SplineTarget::UTGOR_SplineTarget()
:	Super(),
	SplinePoint(ETGOR_SplinePointEnumeration::Between)
{
	Importance = 0.5f;
}

bool UTGOR_SplineTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USplineComponent* Spline = Owner->FindComponentByClass<USplineComponent>();
	if (IsValid(Spline))
	{
		Point.Instance.Component = Spline;

		if (SplinePoint == ETGOR_SplinePointEnumeration::Start)
		{
			Point.Center = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
			Point.Instance.Index = 0;
		}
		else if (SplinePoint == ETGOR_SplinePointEnumeration::End)
		{
			const int32 LastSplinePoint = Spline->GetNumberOfSplineSegments();
			Point.Center = Spline->GetLocationAtSplinePoint(LastSplinePoint, ESplineCoordinateSpace::World);
			Point.Instance.Index = Spline->GetNumberOfSplineSegments();
		}
		else
		{
			const float InputKey = Spline->FindInputKeyClosestToWorldLocation(Origin);
			Point.Center = Spline->GetWorldLocationAtSplinePoint(FMath::RoundToInt(InputKey));
			Point.Instance.Index = FMath::RoundToInt(InputKey);
		}

		// Compute offset vector
		const FTransform Transform = FTransform(Offset) * Spline->GetTransformAtSplinePoint(Point.Instance.Index, ESplineCoordinateSpace::World, true);

		Point.Instance.Offset = Transform.InverseTransformPosition(Origin);
		Point.Distance = Component->ComputeRelativeDistance(Origin, Point.Center);
		return true;

	}
	Point.Distance = 1.0f;
	return false;
}

FVector UTGOR_SplineTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	USplineComponent* Spline = Cast<USplineComponent>(Instance.Component.Get());
	if (IsValid(Spline))
	{
		// Compute offset vector
		const FTransform Transform = FTransform(Offset) * Spline->GetTransformAtSplinePoint(Instance.Index, ESplineCoordinateSpace::World, true);
		return Transform.TransformPosition(Instance.Offset);
	}
	return FVector::ZeroVector;
}

FVector UTGOR_SplineTarget::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	USplineComponent* Spline = Cast<USplineComponent>(Instance.Component.Get());
	if (IsValid(Spline))
	{
		// Compute offset vector
		const FTransform Transform = FTransform(Offset) * Spline->GetTransformAtSplinePoint(Instance.Index, ESplineCoordinateSpace::World, true);
		return Transform.GetLocation();
	}
	return FVector::ZeroVector;
}