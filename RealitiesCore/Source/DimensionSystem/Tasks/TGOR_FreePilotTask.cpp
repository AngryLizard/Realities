// The Gateway of Realities: Planes of Existence.


#include "TGOR_FreePilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_FreePilotTask::UTGOR_FreePilotTask()
{
}

void UTGOR_FreePilotTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_FreePilotTask, Local, COND_None);
}

FTGOR_MovementPosition UTGOR_FreePilotTask::ComputePosition() const
{
	return Local.BaseToPosition(ComputeParentPosition());
}

FTGOR_MovementSpace UTGOR_FreePilotTask::ComputeSpace() const
{
	return Local.BaseToSpace(ComputeParentSpace());
}

void UTGOR_FreePilotTask::InitDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.DynamicToBase(ParentDynamic, Dynamic);
	Super::InitDynamic(Dynamic);
}

void UTGOR_FreePilotTask::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.DynamicToBase(ParentDynamic, Dynamic);
	Super::SimulatePosition(Dynamic);
}

void UTGOR_FreePilotTask::SimulatePosition(const FTGOR_MovementPosition& Position)
{
	const FTGOR_MovementPosition ParentPosition = ComputeParentPosition();
	Local.PositionToBase(ParentPosition, Position);
	Super::SimulatePosition(Position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_FreePilotTask::RepNotifyLocal(const FTGOR_MovementDynamic& Old)
{
	if (IsValid(Identifier.Component))
	{
		ENetRole Role = Identifier.Component->GetOwnerRole();
		if (Role == ENetRole::ROLE_AutonomousProxy)
		{
			if (Old.CompareDynamic(Local, Identifier.Component->AdjustThreshold) < 1.0f)
			{
				Local = Old;
			}
		}
	}
}
