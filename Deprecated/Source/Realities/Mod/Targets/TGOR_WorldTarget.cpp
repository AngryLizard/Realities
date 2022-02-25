// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_WorldTarget.h"

#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "DrawDebugHelpers.h"

UTGOR_WorldTarget::UTGOR_WorldTarget()
:	Super()
{
	Importance = 0.1f;
}

bool UTGOR_WorldTarget::OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	Point.Component = Component;
	Point.Center = Component->GetComponentLocation();
	Point.Distance = 0.99f; // We always hit the environment... faintly.
	return true;
}

bool UTGOR_WorldTarget::SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	UWorld* World = GetWorld();

	FHitResult Hit;
	FCollisionQueryParams Params;
	if (World->LineTraceSingleByObjectType(Hit, Origin, Origin + Direction * MaxDistance, FCollisionObjectQueryParams::AllStaticObjects, Params))
	{
		Point.Center = Hit.Location;
		Point.Component = Component;

		Point.Distance = (1.0f - Hit.Distance / MaxDistance * 0.1f);
		return true;
	}

	Point.Center = Origin + Direction * MaxDistance;
	Point.Component = Component;
	Point.Distance = 0.99f;
	return true;
}

bool UTGOR_WorldTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	if (Point.Component.IsValid())
	{
		// Use interactable itself
		Instance.Component = Point.Component;

		// Compute offset vector
		const FTransform Transform = Instance.Component->GetComponentTransform();
		Instance.Offset = Transform.InverseTransformPosition(Point.Center);

		// Index unused
		Instance.Index = -1;
		return true;
	}
	return false;
}

FVector UTGOR_WorldTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		UTGOR_InteractableComponent* Component = Cast<UTGOR_InteractableComponent>(Instance.Component.Get());
		if (IsValid(Component))
		{
			const FTransform Transform = Component->GetComponentTransform();
			return Transform.TransformPosition(Instance.Offset);
		}
		else
		{
			ERRET("Interactable component invalid", Error, FVector::ZeroVector)
		}
	}
	return FVector::ZeroVector;
}

UTGOR_InteractableComponent* UTGOR_WorldTarget::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	return Cast<UTGOR_InteractableComponent>(Instance.Component.Get());
}

