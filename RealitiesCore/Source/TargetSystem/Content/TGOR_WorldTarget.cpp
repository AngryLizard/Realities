// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_WorldTarget.h"

#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "DrawDebugHelpers.h"

UTGOR_WorldTarget::UTGOR_WorldTarget()
:	Super()
{
	Importance = 0.1f;
}

bool UTGOR_WorldTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	Point.Instance.Component = Component;
	Point.Center = Component->GetComponentLocation();
	Point.Distance = 0.99f; // We always hit the environment... faintly.
	return true;
}

bool UTGOR_WorldTarget::ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	UWorld* World = GetWorld();

	FHitResult Hit;
	FCollisionQueryParams Params;
	if (World->LineTraceSingleByObjectType(Hit, Origin, Origin + Direction * Component->TargetRadius, FCollisionObjectQueryParams::AllStaticObjects, Params))
	{
		Point.Center = Hit.Location;
		Point.Distance = (1.0f - Hit.Distance / Component->TargetRadius * 0.1f);
	}
	else
	{
		Point.Center = Origin + Direction * Component->TargetRadius;
		Point.Distance = 0.99f;
	}

	// Use interactable itself
	Point.Instance.Component = Component;

	// Compute offset vector
	const FTransform Transform = Component->GetComponentTransform();
	Point.Instance.Offset = Transform.InverseTransformPosition(Point.Center);
	return true;
}

FVector UTGOR_WorldTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_AimTargetComponent* Component = Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
	if (IsValid(Component))
	{
		const FTransform Transform = Component->GetComponentTransform();
		return Transform.TransformPosition(Instance.Offset);
	}
	else
	{
		ERRET("Interactable component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

UTGOR_AimTargetComponent* UTGOR_WorldTarget::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	return Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
}

