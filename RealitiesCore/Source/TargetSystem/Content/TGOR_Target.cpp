// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Target.h"

#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "TargetSystem/Components/TGOR_AimComponent.h"

UTGOR_Target::UTGOR_Target()
:	Super(),
Importance(1.0f)
{
}

bool UTGOR_Target::TargetCondition(UTGOR_AimTargetComponent* Component, UTGOR_AimComponent* Source) const
{
	return true;
}

bool UTGOR_Target::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	const FTransform Transform = Component->GetTargetTransform();
	Point.Center = Transform.GetLocation();

	Point.Instance.Component = Component;

	// Compute offset vector
	Point.Instance.Offset = Component->WorldToTarget(Origin);

	// Index unused
	Point.Instance.Index = -1;
	Point.Distance = Component->ComputeRelativeDistance(Origin, Point.Center);
	return true;
}

bool UTGOR_Target::ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const
{
	const FVector Project = Component->ProjectRay(Origin, Direction);
	return OverlapTarget(Component, Project, Point);
}

FVector UTGOR_Target::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	UTGOR_AimTargetComponent* Component = Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
	if (IsValid(Component))
	{
		const FTransform Transform = Component->GetComponentTransform();
		return Transform.TransformPosition(Instance.Offset);
	}
	return FVector::ZeroVector;
}

FVector UTGOR_Target::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	return QueryAimLocation(Instance);
}

FTGOR_Display UTGOR_Target::QueryDisplay(const FTGOR_AimInstance& Instance) const
{
	return GetDisplay();
}

UTGOR_AimTargetComponent* UTGOR_Target::QueryInteractable(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		AActor* TargetOwner = Instance.Component->GetOwner();
		if (IsValid(TargetOwner))
		{
			UActorComponent* Component = TargetOwner->GetComponentByClass(UTGOR_AimTargetComponent::StaticClass());
			return Cast<UTGOR_AimTargetComponent>(Instance.Component.Get());
		}
	}
	return nullptr;
}
