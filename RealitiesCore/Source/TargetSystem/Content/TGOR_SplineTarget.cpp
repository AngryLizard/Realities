// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_SplineTarget.h"
#include "Engine/Classes/Components/SplineComponent.h"

#include "../Components/TGOR_AimTargetComponent.h"

UTGOR_SplineTarget::UTGOR_SplineTarget()
:	Super(),
	SplinePoint(ETGOR_SplinePointEnumeration::Between),
	DistanceThreshold(50.0f)
{
	Importance = 0.5f;
}

bool UTGOR_SplineTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USplineComponent* Spline = Owner->FindComponentByClass<USplineComponent>();
	if(IsValid(Spline))
	{
		Point.Component = Spline;

		if (SplinePoint == ETGOR_SplinePointEnumeration::Start)
		{
			const FVector Location = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
			return ComputeDistance(Location, Origin, MaxDistance, Point);
		}
		else if (SplinePoint == ETGOR_SplinePointEnumeration::End)
		{
			const int32 LastSplinePoint = Spline->GetNumberOfSplineSegments();
			const FVector Location = Spline->GetLocationAtSplinePoint(LastSplinePoint, ESplineCoordinateSpace::World);
			return ComputeDistance(Location, Origin, MaxDistance, Point);
		}

		const FVector Location = Spline->FindLocationClosestToWorldLocation(Origin, ESplineCoordinateSpace::World);
		return ComputeDistance(Location, Origin, MaxDistance, Point);
	}
	Point.Distance = 1.0f;
	return false;
}

bool UTGOR_SplineTarget::SearchTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USplineComponent* Spline = Owner->FindComponentByClass<USplineComponent>();
	if (IsValid(Spline))
	{
		const float ScaledThreshold = DistanceThreshold * Component->GetComponentScale().GetMax();

		Point.Component = Spline;

		if (SplinePoint == ETGOR_SplinePointEnumeration::Start)
		{
			Point.Center = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		}
		else if (SplinePoint == ETGOR_SplinePointEnumeration::End)
		{
			const int32 LastSplinePoint = Spline->GetNumberOfSplineSegments();
			Point.Center = Spline->GetLocationAtSplinePoint(LastSplinePoint, ESplineCoordinateSpace::World);
		}
		else
		{
			const float InputKey = Spline->FindInputKeyClosestToWorldLocation(Origin);
			Point.Center = Spline->GetWorldLocationAtSplinePoint(FMath::RoundToInt(InputKey));
		}

		// Project onto bone screen plane if not hit
		const FVector Relative = ComputeProject(Point.Center, Origin, Direction);
		Point.Distance = Relative.Size() / ScaledThreshold;
		return Point.Distance < 1.0f;

	}
	Point.Distance = 1.0f;
	return false;
}

bool UTGOR_SplineTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	USplineComponent* Spline = Cast<USplineComponent>(Point.Component.Get());
	if (IsValid(Spline))
	{
		// Use interactable itself
		Instance.Component = Point.Component;

		// Index used to denote spline point
		if (SplinePoint == ETGOR_SplinePointEnumeration::Start)
		{
			Instance.Index = 0;
		}
		else if (SplinePoint == ETGOR_SplinePointEnumeration::End)
		{
			Instance.Index = Spline->GetNumberOfSplineSegments();
		}
		else
		{
			const float InputKey = Spline->FindInputKeyClosestToWorldLocation(Point.Center);
			Instance.Index = FMath::RoundToInt(InputKey);
		}

		// Compute offset vector
		const FTransform Transform = Spline->GetTransformAtSplinePoint(Instance.Index, ESplineCoordinateSpace::World, true);
		Instance.Offset = ComputeOffset(Transform, Origin, Direction);
		return true;
	}
	return false;
}

FVector UTGOR_SplineTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	USplineComponent* Spline = Cast<USplineComponent>(Instance.Component.Get());
	if (IsValid(Spline))
	{
		// Compute offset vector
		const FTransform Transform = Spline->GetTransformAtSplinePoint(Instance.Index, ESplineCoordinateSpace::World, true);
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
		const FTransform Transform = Spline->GetTransformAtSplinePoint(Instance.Index, ESplineCoordinateSpace::World, true);
		return Transform.GetLocation();
	}
	return FVector::ZeroVector;
}