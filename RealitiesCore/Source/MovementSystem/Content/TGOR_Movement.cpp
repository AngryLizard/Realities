// The Gateway of Realities: Planes of Existence.


#include "TGOR_Movement.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "MovementSystem/Tasks/TGOR_MovementTask.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"
#include "AnimationSystem/Content/TGOR_Animation.h"

UTGOR_Movement::UTGOR_Movement()
	: Super(),
	Mode(ETGOR_MovementEnumeration::Queued)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MovementTask* UTGOR_Movement::CreateMovementTask(UTGOR_MovementComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Movement::TaskInitialise(UTGOR_MovementTask* MovementTask)
{
	OnTaskInitialise(MovementTask);
}
