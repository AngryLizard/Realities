// The Gateway of Realities: Planes of Existence.

#include "TGOR_Pilot.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "DimensionSystem/Tasks/TGOR_PilotTask.h"


UTGOR_Pilot::UTGOR_Pilot()
	: Super()
{
}

void UTGOR_Pilot::BuildResource()
{
	if (!Abstract)
	{
		StaticTask = UTGOR_Task::CreateTask<UTGOR_PilotComponent>(this, TaskType);
	}
	Super::BuildResource();
}

bool UTGOR_Pilot::Validate_Implementation()
{
	return UTGOR_Task::TaskValidation(TaskType, StaticTask) && Super::Validate_Implementation();
}

UTGOR_PilotTask* UTGOR_Pilot::CreatePilotTask(UTGOR_PilotComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

void UTGOR_Pilot::TaskInitialise(UTGOR_PilotTask* PilotTask)
{
	OnTaskInitialise(PilotTask);
}
