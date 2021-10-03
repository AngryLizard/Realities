// The Gateway of Realities: Planes of Existence.

#include "TGOR_Pilot.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "DimensionSystem/Tasks/TGOR_PilotTask.h"


UTGOR_Pilot::UTGOR_Pilot()
	: Super()
{
}

UTGOR_PilotTask* UTGOR_Pilot::CreatePilotTask(UTGOR_PilotComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

void UTGOR_Pilot::TaskInitialise(UTGOR_PilotTask* PilotTask)
{
	OnTaskInitialise(PilotTask);
}
