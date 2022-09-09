// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_ComponentTarget.h"

#include "../Components/TGOR_AimTargetComponent.h"

UTGOR_ComponentTarget::UTGOR_ComponentTarget()
:	Super()
{
	Importance = 1.0f;
}

bool UTGOR_ComponentTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;
	const float ScaledThreshold = Component->TargetRadius * DistanceThreshold * Component->GetComponentScale().GetMax();

	Point.Component = Component;

	// When overlapping we take the interactable radius into consideration
	const FTransform Transform = FTransform(Offset) * Component->GetComponentTransform();
	return ComputeDistance(Transform.GetLocation(), Origin, MaxDistance + ScaledThreshold, Point);
}


bool UTGOR_ComponentTarget::SearchTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;
	const float ScaledThreshold = Component->TargetRadius * DistanceThreshold * Component->GetComponentScale().GetMax();

	Point.Component = Component;

	const FTransform Transform = FTransform(Offset) * Component->GetComponentTransform();
	Point.Center = Transform.GetLocation();

	// Project onto view plane and transform to relative offset
	const FVector Relative = ComputeProject(Point.Center, Origin, Direction);
	Point.Distance = Relative.Size() / ScaledThreshold;
	return Point.Distance < 1.0f;
}

bool UTGOR_ComponentTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	UTGOR_AimTargetComponent* Component = Cast<UTGOR_AimTargetComponent>(Point.Component.Get());
	if (IsValid(Component))
	{
		// Use interactable itself
		Instance.Component = Point.Component;

		// Compute offset vector
		const FTransform Transform = FTransform(Offset) * Component->GetComponentTransform();
		Instance.Offset = ComputeOffset(Transform, Origin, Direction);

		// Index unused
		Instance.Index = -1;
		return true;
	}
	return false;
}

FVector UTGOR_ComponentTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_AimTargetComponent* Component = Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
	if (IsValid(Component))
	{
		const FTransform Transform = FTransform(Offset) * Component->GetComponentTransform();
		return Transform.TransformPosition(Instance.Offset);
	}
	else
	{
		ERRET("Interactable component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

FVector UTGOR_ComponentTarget::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_AimTargetComponent* Component = Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
	if (IsValid(Component))
	{
		const FTransform Transform = FTransform(Offset) * Component->GetComponentTransform();
		return Transform.GetLocation();
	}
	else
	{
		ERRET("Interactable component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

UTGOR_AimTargetComponent* UTGOR_ComponentTarget::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	return Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
}
