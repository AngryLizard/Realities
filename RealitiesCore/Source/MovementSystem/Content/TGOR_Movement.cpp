// The Gateway of Realities: Planes of Existence.


#include "TGOR_Movement.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "MovementSystem/Tasks/TGOR_MovementTask.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

UTGOR_Movement::UTGOR_Movement()
	: Super(),
	CanRotateOnCollision(false),
	Mode(ETGOR_MovementEnumeration::Queued)
{
}

void UTGOR_Movement::BuildResource()
{
	if (!Abstract)
	{
		StaticTask = UTGOR_Task::CreateTask<UTGOR_MovementComponent>(this, TaskType);
	}
	Super::BuildResource();
}

void UTGOR_Movement::PostBuildResource()
{
	Super::PostBuildResource();

	// Override inserted
	TArray<UTGOR_Movement*> Movements = Instanced_OverrideInsertions.GetListOfType<UTGOR_Movement>();//GetIListFromType<UTGOR_Movement>();
	for (UTGOR_Movement* Movement : Movements)
	{
		Movement->OverrideMovements(this);
	}
}

bool UTGOR_Movement::Validate_Implementation()
{
	return UTGOR_Task::TaskValidation(TaskType, StaticTask) && Super::Validate_Implementation();
}

void UTGOR_Movement::OverrideMovements(UTGOR_Movement* Override)
{
	// Check for cycles
	if (Override == this)
	{
		ERROR(FString("Movement override cycle found in ") + GetDefaultName(), Error);
	}

	// Don't need to add if already there (Should never happen with no cycles, but ye know)
	if (!OverriddenBy.Contains(Override))
	{
		// Add to list
		OverriddenBy.Emplace(Override);

		// Add to children
		TArray<UTGOR_Movement*> Movements = Instanced_OverrideInsertions.GetListOfType<UTGOR_Movement>();//GetIListFromType<UTGOR_Movement>();
		for (UTGOR_Movement* Movement : Movements)
		{
			Movement->OverrideMovements(Override);
		}
	}
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

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Movement::IsOverriddenBy(UTGOR_Movement* Movement) const
{
	return OverriddenBy.Contains(Movement);
}


void UTGOR_Movement::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(OverrideInsertions);
	MOV_INSERTION(AttributeInsertions);
}