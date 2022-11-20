// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_ComponentTarget.h"

#include "../Components/TGOR_AimTargetComponent.h"

UTGOR_ComponentTarget::UTGOR_ComponentTarget()
:	Super()
{
	Importance = 1.0f;
}

bool UTGOR_ComponentTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	const FTransform Transform = FTransform(Offset) * Component->GetTargetTransform();
	Point.Center = Transform.GetLocation();

	Point.Instance.Component = Component;

	// Compute offset vector
	Point.Instance.Offset = Component->WorldToTarget(Origin);

	// Index unused
	Point.Instance.Index = -1;
	Point.Distance = Component->ComputeRelativeDistance(Origin, Point.Center, DistanceThreshold);
	return true;
}

FVector UTGOR_ComponentTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_AimTargetComponent* Component = Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
	if (IsValid(Component))
	{
		const FTransform Transform = FTransform(Offset) * Component->GetTargetTransform();
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
		const FTransform Transform = FTransform(Offset) * Component->GetTargetTransform();
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
