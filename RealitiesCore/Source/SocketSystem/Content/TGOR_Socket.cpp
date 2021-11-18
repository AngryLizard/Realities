// The Gateway of Realities: Planes of Existence.

#include "TGOR_Socket.h"
#include "TargetSystem/Components/TGOR_InteractableComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

UTGOR_Socket::UTGOR_Socket()
	: Super(),
	DistanceThreshold(150.0f),
	TargetOffset(FVector::ZeroVector),
	AllowsReparent(true)
{
}

UTGOR_MobilityComponent* UTGOR_Socket::QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const
{
	AActor* Owner = Component->GetOwner();
	UTGOR_MobilityComponent* Mobility = Owner->FindComponentByClass<UTGOR_MobilityComponent>();
	if (IsValid(Mobility))
	{
		Position = Mobility->ComputePosition();
	}
	return Mobility;
}

UTGOR_MobilityComponent* UTGOR_Socket::QuerySocketWith(UActorComponent* Component, FTGOR_MovementPosition& Position) const
{
	UTGOR_MobilityComponent* Mobility = Cast<UTGOR_MobilityComponent>(Component);
	if (IsValid(Mobility))
	{
		Position = Mobility->ComputePosition();
	}
	return Mobility;
}

/*

bool UTGOR_Socket::OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	FTGOR_MovementPosition Position;
	UTGOR_MobilityComponent* Mobility = QuerySocket(Component, Position);
	if (IsValid(Mobility))
	{
		Point.Component = Mobility;

		return ComputeDistance(Position.Linear, Origin, MaxDistance, Point);
	}
	return false;
}

bool UTGOR_Socket::SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	FTGOR_MovementPosition Position;
	UTGOR_MobilityComponent* Mobility = QuerySocket(Component, Position);
	if (IsValid(Mobility))
	{
		const float Scale = Component->GetComponentScale().GetMax();
		const float ScaledThreshold = DistanceThreshold * Scale;

		Point.Component = Mobility;

		// Get socket transform (locally offset)
		Point.Center = Position.Linear + Position.Angular * TargetOffset * Scale;

		// Check whether target is close enough
		if ((Point.Center - Origin).SizeSquared() < (MaxDistance * MaxDistance))
		{
			// Project onto socket screen plane if not hit
			const FVector Relative = ComputeProject(Point.Center, Origin, Direction);
			Point.Distance = Relative.Size() / ScaledThreshold;
			return Point.Distance < 1.0f;
		}
	}
	return false;
}

bool UTGOR_Socket::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	FTGOR_MovementPosition Position;
	UTGOR_MobilityComponent* Mobility = QuerySocketWith(Point.Component.Get(), Position);
	if (IsValid(Mobility))
	{
		Instance.Component = Mobility;

		// Get socket transform (locally offset)
		//Instance.Index = Mobility->GetSocketIndex(const_cast<UTGOR_Socket*>(this));
		const FTransform Transform = FTransform(Position.Angular, Position.Linear);

		// Ignore local offset for creation
		Instance.Offset = ComputeOffset(Transform, Origin, Direction);
		return true;
	}
	return false;
}

FVector UTGOR_Socket::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		FTGOR_MovementPosition Position;
		UTGOR_MobilityComponent* Mobility = QuerySocketWith(Instance.Component.Get(), Position);
		if (IsValid(Mobility))
		{
			// Project onto socket screen plane
			const FTransform Transform = FTransform(Position.Angular, Position.Linear);
			return Transform.TransformPosition(Instance.Offset);
		}
		else
		{
			ERRET("Skeletal component invalid", Error, FVector::ZeroVector)
		}
	}
	return(FVector::ZeroVector);
}

FVector UTGOR_Socket::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		FTGOR_MovementPosition Position;
		UTGOR_MobilityComponent* Mobility = QuerySocketWith(Instance.Component.Get(), Position);
		if (IsValid(Mobility))
		{

			// Project onto socket screen plane
			return Position.Linear;
		}
		else
		{
			ERRET("Skeletal component invalid", Error, FVector::ZeroVector)
		}
	}
	return(FVector::ZeroVector);
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Socket::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(TargetInsertions);
}
