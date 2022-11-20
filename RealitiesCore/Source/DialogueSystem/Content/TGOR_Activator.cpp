// The Gateway of Realities: Planes of Existence.

#include "TGOR_Activator.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DialogueSystem/Tasks/TGOR_ActivatorTask.h"
#include "DialogueSystem/Components/TGOR_ActivatorComponent.h"

UTGOR_Activator::UTGOR_Activator()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ActivatorTask* UTGOR_Activator::CreateActivatorTask(UTGOR_ActivatorComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Activator::TaskInitialise(UTGOR_ActivatorTask* Task)
{
	OnTaskInitialise(Task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
