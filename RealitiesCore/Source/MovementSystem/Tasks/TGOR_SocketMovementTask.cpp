// The Gateway of Realities: Planes of Existence.


#include "TGOR_SocketMovementTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "SocketSystem/Tasks/TGOR_SocketPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_SocketMovementTask::UTGOR_SocketMovementTask()
	: Super(),
	RootComponent(nullptr),
	SocketTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SocketMovementTask::Initialise()
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
	
	SocketTask.Reset();
	return true;
}

bool UTGOR_SocketMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	UTGOR_SocketPilotTask* Task = SocketTask.IsValid() ? SocketTask.Get() : RootComponent->GetCurrentPOfType<UTGOR_SocketPilotTask>();
	if (!RootComponent.IsValid() || !IsValid(Task))
	{
		return false;
	}

	return true;
}

void UTGOR_SocketMovementTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	Super::Update(Space, External, Tick, Output);
}

void UTGOR_SocketMovementTask::PrepareStart()
{
	Super::PrepareStart();

	SocketTask = RootComponent->GetCurrentPOfType<UTGOR_SocketPilotTask>();
}

void UTGOR_SocketMovementTask::Interrupt()
{
	Super::Interrupt();

	SocketTask.Reset();
}
