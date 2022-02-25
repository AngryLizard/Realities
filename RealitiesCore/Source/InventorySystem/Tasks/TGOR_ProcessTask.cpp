// The Gateway of Realities: Planes of Existence.

#include "TGOR_ProcessTask.h"
#include "InventorySystem/Components/TGOR_ProcessComponent.h"
#include "InventorySystem/Content/TGOR_Process.h"

UTGOR_ProcessTask::UTGOR_ProcessTask()
	: Super()
{
}

void UTGOR_ProcessTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ProcessTask, Identifier, COND_InitialOnly);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Process* UTGOR_ProcessTask::GetProcess() const
{
	return Identifier.Content;
}


UTGOR_ProcessComponent* UTGOR_ProcessTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_ProcessTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ProcessTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);
	OnInitialise();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
