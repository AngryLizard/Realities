// The Gateway of Realities: Planes of Existence.

#include "TGOR_MontageMovementTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Tasks/TGOR_AttachedPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_MontageMovementTask::UTGOR_MontageMovementTask()
	: Super(),
	RootComponent(nullptr),
	AttachedTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MontageMovementTask::Initialise()
{
	Super::Initialise();

	RootComponent = Identifier.Component->GetRootPilot();
	if (!RootComponent.IsValid())
	{
		ERROR("RootComponent invalid", Error);
	}

	AttachedTask.Reset();
}

bool UTGOR_MontageMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	UTGOR_AttachedPilotTask* Task = RootComponent->GetCurrentPOfType<UTGOR_AttachedPilotTask>();
	if (!RootComponent.IsValid() || !IsValid(Task))
	{
		return false;
	}

	return true;
}

void UTGOR_MontageMovementTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	const FTGOR_MovementPosition Offset = TickAnimationRootMotion(Tick.DeltaTime);

	// Simulate move
	FTGOR_MovementSpace Out = Space;
	RootComponent->SimulateMove(Out, Offset, Tick.DeltaTime, true);
	AttachedTask->SimulateDynamic(Out);

	Super::Update(Space, External, Tick, Output);
}

void UTGOR_MontageMovementTask::PrepareStart()
{
	Super::PrepareStart();

	AttachedTask = RootComponent->GetCurrentPOfType<UTGOR_AttachedPilotTask>();
	if (!AttachedTask.IsValid())
	{
		ERROR("AttachedTask invalid", Fatal);
	}
}

void UTGOR_MontageMovementTask::Interrupt()
{
	AttachedTask.Reset();
	Super::Interrupt();
}
