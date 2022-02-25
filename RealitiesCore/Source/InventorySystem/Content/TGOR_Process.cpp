// The Gateway of Realities: Planes of Existence.

#include "TGOR_Process.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "InventorySystem/Tasks/TGOR_ProcessTask.h"
#include "InventorySystem/Components/TGOR_ProcessComponent.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"

UTGOR_Process::UTGOR_Process()
	: Super()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ProcessTask* UTGOR_Process::CreateProcessTask(UTGOR_ProcessComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Process::TaskInitialise(UTGOR_ProcessTask* ProcessTask)
{
	OnTaskInitialise(ProcessTask);
}
