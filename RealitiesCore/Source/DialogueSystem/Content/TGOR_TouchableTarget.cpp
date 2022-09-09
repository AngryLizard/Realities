// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_TouchableTarget.h"

#include "DialogueSystem/Components/TGOR_TouchableComponent.h"

UTGOR_TouchableTarget::UTGOR_TouchableTarget()
:	Super()
{
	Importance = 1.0f;
}

bool UTGOR_TouchableTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;
	const float ScaledThreshold = Component->TargetRadius * Component->GetComponentScale().GetMax();

	Point.Component = Component;

	// For now ignore target domain and just use target
	return ComputeDistance(Component->GetComponentLocation(), Origin, MaxDistance + ScaledThreshold, Point);
}


bool UTGOR_TouchableTarget::SearchTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	UTGOR_TouchableComponent* Touchable = Cast<UTGOR_TouchableComponent>(Component);
	if (!IsValid(Touchable))
	{
		return false;
	}

	Point.Component = Touchable;

	Touchable->GetTargetProjection(Origin, Direction, Point.Center, Point.Distance);
	return Touchable->GetTargetProjection(Origin, Direction, Point.Center, Point.Distance);
}

bool UTGOR_TouchableTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	UTGOR_TouchableComponent* Touchable = Cast<UTGOR_TouchableComponent>(Point.Component.Get());
	if (IsValid(Touchable))
	{
		// Use interactable itself
		Instance.Component = Point.Component;

		// Compute offset vector
		const FTransform Transform = Touchable->GetComponentTransform();
		Instance.Offset = ComputeOffset(Transform, Origin, Direction);

		// Index unused
		Instance.Index = -1;
		return true;
	}
	return false;
}

FVector UTGOR_TouchableTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_TouchableComponent* Touchable = Cast<UTGOR_TouchableComponent>(Instance.Component.Get());
	if (IsValid(Touchable))
	{
		const FTransform Transform = Touchable->GetComponentTransform();
		return Transform.TransformPosition(Instance.Offset);
	}
	else
	{
		ERRET("Touchable component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

FVector UTGOR_TouchableTarget::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_TouchableComponent* Touchable = Cast<UTGOR_TouchableComponent>(Instance.Component.Get());
	if (IsValid(Touchable))
	{
		const FTransform Transform = Touchable->GetComponentTransform();
		return Transform.GetLocation();
	}
	else
	{
		ERRET("Interactable component invalid", Error, FVector::ZeroVector)
	}
	return FVector::ZeroVector;
}

UTGOR_AimTargetComponent* UTGOR_TouchableTarget::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	return Cast<UTGOR_TouchableComponent>(Instance.Component.Get());
}
