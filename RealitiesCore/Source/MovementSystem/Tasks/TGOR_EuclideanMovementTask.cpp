// The Gateway of Realities: Planes of Existence.


#include "TGOR_EuclideanMovementTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Tasks/TGOR_EuclideanPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_EuclideanMovementTask::UTGOR_EuclideanMovementTask()
	: Super(),
	RootComponent(nullptr),
	EuclideanTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_EuclideanMovementTask::Initialise()
{
	if (!Super::Initialise())
	{
		return false;
	}

	RootComponent = Identifier.Component->GetRootPilot();
	if (!RootComponent.IsValid())
	{
		ERRET("RootComponent invalid", Error, false);
	}

	EuclideanTask.Reset();
	return true;
}

bool UTGOR_EuclideanMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	UTGOR_EuclideanPilotTask* Task = RootComponent->GetCurrentPOfType<UTGOR_EuclideanPilotTask>();
	if (!RootComponent.IsValid() || !IsValid(Task))
	{
		return false;
	}

	return true;
}

void UTGOR_EuclideanMovementTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	SimulateSymplectic(Space, Output, External, Tick.DeltaTime, true);
	Super::Update(Space, External, Tick, Output);
}

void UTGOR_EuclideanMovementTask::SimulateSymplectic(const FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementExternal& External, float DeltaTime, bool Sweep)
{
	// Make sure we aren't introducing NaNs
	if (Force.Force.ContainsNaN())
	{
		ERROR("Output force NAN!", Error);
	}

	if (Force.Torque.ContainsNaN())
	{
		ERROR("Output torque NAN!", Error);
	}

	// Apply forces to current physics state
	const FTGOR_MovementBody& MovementBody = RootComponent->GetBody();
	FTGOR_MovementSpace Out = MovementBody.SimulateForce(Space, Force, External, DeltaTime);

	// Consume root motion
	const FTGOR_MovementPosition Offset = TickAnimationRootMotion(Out, DeltaTime);

	// Simulate move
	RootComponent->SimulateMove(Out, Offset, DeltaTime, Sweep, Elasticity.Value, Friction.Value, LastMovementImpact);
	EuclideanTask->SimulateDynamic(Out);

	if (!FMath::IsNearlyZero(LastMovementImpact.Strength))
	{
		OnSymplecticCollision(Space, LastMovementImpact.Normal, LastMovementImpact.Strength, DeltaTime);
	}
}

void UTGOR_EuclideanMovementTask::PrepareStart()
{
	Super::PrepareStart();

	EuclideanTask = RootComponent->GetCurrentPOfType<UTGOR_EuclideanPilotTask>();
	if (!EuclideanTask.IsValid())
	{
		ERROR("EuclideanTask invalid", Fatal);
	}
}

void UTGOR_EuclideanMovementTask::Interrupt()
{
	EuclideanTask.Reset();
	Super::Interrupt();
}
