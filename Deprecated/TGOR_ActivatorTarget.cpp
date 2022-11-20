// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_ActivatorTarget.h"

#include "DialogueSystem/Components/TGOR_ActivatorComponent.h"

UTGOR_ActivatorTarget::UTGOR_ActivatorTarget()
:	Super()
{
	Importance = 1.0f;
}

bool UTGOR_ActivatorTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;
	const float ScaledThreshold = Component->TargetRadius * Component->GetComponentScale().GetMax();

	Point.Component = Component;

	// For now ignore target domain and just use target
	return ComputeDistance(Component->GetComponentLocation(), Origin, MaxDistance + ScaledThreshold, Point);
}


bool UTGOR_ActivatorTarget::SearchTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	UTGOR_ActivatorComponent* Activator = Cast<UTGOR_ActivatorComponent>(Component);
	if (!IsValid(Activator))
	{
		return false;
	}

	Point.Component = Activator;

	return Activator->GetTargetProjection(Origin, Direction, Point.Center, Point.Distance);
}

bool UTGOR_ActivatorTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	UTGOR_ActivatorComponent* Activator = Cast<UTGOR_ActivatorComponent>(Point.Component.Get());
	if (IsValid(Activator))
	{
		// Use interactable itself
		Instance.Component = Point.Component;

		// Compute offset vector
		const FTransform Transform = Activator->GetComponentTransform();
		Instance.Offset = ComputeOffset(Transform, Origin, Direction);

		// Index unused
		Instance.Index = -1;
		return true;
	}
	return false;
}

FVector UTGOR_ActivatorTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_ActivatorComponent* Activator = Cast<UTGOR_ActivatorComponent>(Instance.Component.Get());
	if (IsValid(Activator))
	{
		const FTransform Transform = Activator->GetComponentTransform();
		return Transform.TransformPosition(Instance.Offset);
	}
	else
	{
		ERRET("Activator component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

FVector UTGOR_ActivatorTarget::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_ActivatorComponent* Activator = Cast<UTGOR_ActivatorComponent>(Instance.Component.Get());
	if (IsValid(Activator))
	{
		const FTransform Transform = Activator->GetComponentTransform();
		return Transform.GetLocation();
	}
	else
	{
		ERRET("Interactable component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

UTGOR_AimTargetComponent* UTGOR_ActivatorTarget::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	return Cast<UTGOR_ActivatorComponent>(Instance.Component.Get());
}
