// The Gateway of Realities: Planes of Existence.


#include "TGOR_EuclideanMovementTask.h"
#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Tasks/TGOR_EuclideanPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_EuclideanMovementTask::UTGOR_EuclideanMovementTask()
	: Super(),
	RigidComponent(nullptr),
	EuclideanTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EuclideanMovementTask::Initialise()
{
	Super::Initialise();

	RigidComponent = Identifier.Component->GetOwnerRootScene<UTGOR_RigidComponent>();
	if (IsValid(RigidComponent))
	{
		EuclideanTask = RigidComponent->GetPOfType<UTGOR_EuclideanPilotTask>();
	}
}

bool UTGOR_EuclideanMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}
	return IsValid(RigidComponent) && IsValid(EuclideanTask) && EuclideanTask->IsRegistered();
}

void UTGOR_EuclideanMovementTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	Super::Update(Space, External, Tick, Output);

	////////////////////////////////////////////////////////////////////////////////////////////
	const FTGOR_MovementBody& MovementBody = RigidComponent->GetBody();
	RigidComponent->SimulateSymplectic(Space, Output, External, Tick.Deltatime, true);

	EuclideanTask->SimulateDynamic(Space);
}
