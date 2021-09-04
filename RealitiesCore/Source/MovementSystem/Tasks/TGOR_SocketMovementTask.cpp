// The Gateway of Realities: Planes of Existence.


#include "TGOR_SocketMovementTask.h"
#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "SocketSystem/Tasks/TGOR_SocketPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_SocketMovementTask::UTGOR_SocketMovementTask()
	: Super(),
	RigidComponent(nullptr),
	SocketTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SocketMovementTask::Initialise()
{
	Super::Initialise();

	RigidComponent = Identifier.Component->GetOwnerRootScene<UTGOR_RigidComponent>();
	if (IsValid(RigidComponent))
	{
		SocketTask = RigidComponent->GetPOfType<UTGOR_SocketPilotTask>();
	}
}

bool UTGOR_SocketMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}
	return IsValid(RigidComponent) && IsValid(SocketTask) && SocketTask->IsRegistered();
}

void UTGOR_SocketMovementTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	Super::Update(Space, External, Tick, Output);
}
