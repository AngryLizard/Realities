// The Gateway of Realities: Planes of Existence.


#include "TGOR_SlidingTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Tasks/TGOR_LinearPilotTask.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_SlidingTask::UTGOR_SlidingTask()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SlidingTask::Initialise()
{
	return Super::Initialise();
}

bool UTGOR_SlidingTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	return true;
}

void UTGOR_SlidingTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	Super::Reset(Space, External);
}

void UTGOR_SlidingTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	Super::QueryInput(OutInput, OutView);
}

void UTGOR_SlidingTask::Context(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick)
{
	Super::Context(Space, External, Tick);


	FVector QueryVector = Space.RelativeLinearVelocity;

	const FTGOR_MovementBody& Body = RootComponent->GetBody();
	if (!FMath::IsNearlyZero(SlidingDirection.SquaredLength(), 0.01))
	{
		QueryVector = SlidingDirection;
	}

	if (!FMath::IsNearlyZero(QueryVector.SquaredLength(), 0.01))
	{
		const FVector QueryDirection = QueryVector.GetSafeNormal();
		const FVector SlidingVelocity = FVector::VectorPlaneProject(Space.RelativeLinearVelocity, QueryDirection);

		FHitResult Hit;
		const FVector Translation = QueryVector * SlidingQueryDistance;
		const double Radius = FMath::Sqrt(FMath::Square(Body.Radius) + FMath::Square(Body.Height / 2));
		if (RootComponent->MovementSphereTraceSweep(Radius, Space.Linear, Translation, Hit))
		{
			const FVector AnchorPoint = Hit.Location;
			SlidingDirection = -Hit.Normal;
			SlidingDistance = Hit.Distance;

			const FVector QueryOffset = SlidingVelocity * (SlidingQuerySpread / MaxSlidingSpeed);
			if (RootComponent->MovementSphereTraceSweep(Radius, Space.Linear, Translation + QueryOffset, Hit))
			{
				const FVector Projection = (Hit.Location - AnchorPoint).GetSafeNormal();
				if (!FMath::IsNearlyZero(Projection | SlidingDirection))
				{
					SlidingDirection = FVector::VectorPlaneProject(SlidingDirection, Projection).GetSafeNormal();
				}
			}
		}
	}
}

void UTGOR_SlidingTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();


	Super::Update(Space, External, Tick, Output);
}
