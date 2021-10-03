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

void UTGOR_EuclideanMovementTask::Initialise()
{
	Super::Initialise();

	RootComponent = Identifier.Component->GetRootPilot();
	if (IsValid(RootComponent))
	{
		EuclideanTask = RootComponent->GetPOfType<UTGOR_EuclideanPilotTask>();
	}
	else
	{
		ERROR("RootComponent invalid", Error);
	}
}

bool UTGOR_EuclideanMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}
	return IsValid(RootComponent) && IsValid(EuclideanTask) && EuclideanTask->IsRegistered();
}

void UTGOR_EuclideanMovementTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	Super::Update(Space, External, Tick, Output);

	////////////////////////////////////////////////////////////////////////////////////////////

	RootComponent->SimulateSymplectic(Space, Output, External, Tick.Deltatime, true);
	EuclideanTask->SimulateDynamic(Space);
}
