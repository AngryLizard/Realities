// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Target.h"

#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"

UTGOR_Target::UTGOR_Target()
:	Super(),
Importance(1.0f)
{
}

bool UTGOR_Target::OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	Point.Component = Component;
	return ComputeDistance(Component->GetComponentLocation(), Origin, MaxDistance, Point);
}

bool UTGOR_Target::SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	Point.Component = Component;
	Point.Center = Component->GetComponentLocation();

	// Project onto view plane and transform to relative offset
	const FVector Relative = ComputeProject(Point.Center, Origin, Direction);
	Point.Distance = Relative.Size() / MaxDistance;
	return Point.Distance < 1.0f;
}

bool UTGOR_Target::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	if (Point.Component.IsValid())
	{
		// Use interactable itself
		Instance.Component = Point.Component;

		// Compute offset vector
		const FTransform Transform = Instance.Component->GetComponentTransform();
		Instance.Offset = ComputeOffset(Transform, Origin, Direction);

		// Index unused
		Instance.Index = -1;
		return true;
	}
	return false;
}

FVector UTGOR_Target::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		const FTransform Transform = Instance.Component->GetComponentTransform();
		return Transform.TransformPosition(Instance.Offset);
	}
	return FVector::ZeroVector;
}

FVector UTGOR_Target::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	return QueryAimLocation(Instance);
}

FVector UTGOR_Target::ComputeOffset(const FTransform& Transform, const FVector& Origin, const FVector& Direction) const
{
	// Compute distance vector
	const FVector Location = Transform.GetLocation();
	const FVector Diff = (Location - Origin);
	const float Distance = Diff | Direction;

	// Project onto view plane and transform to relative offset
	const FVector Target = Origin + Direction * Distance;
	return Transform.InverseTransformPosition(Target);
}

FTGOR_Display UTGOR_Target::QueryDisplay(const FTGOR_AimInstance& Instance) const
{
	return GetDisplay();
}

UTGOR_InteractableComponent* UTGOR_Target::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		AActor* TargetOwner = Instance.Component->GetOwner();
		if (IsValid(TargetOwner))
		{
			UActorComponent* Component = TargetOwner->GetComponentByClass(UTGOR_InteractableComponent::StaticClass()); 
			return Cast<UTGOR_InteractableComponent>(Instance.Component.Get());
		}
	}
	return nullptr;
}

FVector UTGOR_Target::ComputeProject(const FVector& Location, const FVector& Origin, const FVector& Direction) const
{
	// Compute distance vector
	const FVector Diff = (Location - Origin);
	const float Distance = FMath::Max(Diff | Direction, 0.0f);

	// Project onto view plane and transform to relative offset
	return Diff - Direction * Distance;
}

bool UTGOR_Target::ComputeDistance(const FVector& Location, const FVector& Origin, float Radius, FTGOR_AimPoint& Point) const
{
	Point.Center = Location;

	const float Distance = (Point.Center - Origin).Size();
	Point.Distance = Distance / Radius;
	return Point.Distance < 1.0f;
}