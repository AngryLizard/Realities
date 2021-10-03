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

void UTGOR_SocketMovementTask::Initialise()
{
	Super::Initialise();

	RootComponent = Identifier.Component->GetRootPilot();
	if (IsValid(RootComponent))
	{
		SocketTask = RootComponent->GetPOfType<UTGOR_SocketPilotTask>();
	}
	else
	{
		ERROR("RootComponent invalid", Error);
	}
}

bool UTGOR_SocketMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}
	return IsValid(RootComponent) && IsValid(SocketTask) && SocketTask->IsRegistered();
}

void UTGOR_SocketMovementTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	Super::Update(Space, External, Tick, Output);
}
