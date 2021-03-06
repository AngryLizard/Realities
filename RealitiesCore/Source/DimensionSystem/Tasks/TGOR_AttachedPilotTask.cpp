// The Gateway of Realities: Planes of Existence.


#include "TGOR_AttachedPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_AttachedPilotTask::UTGOR_AttachedPilotTask()
{
}

void UTGOR_AttachedPilotTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_AttachedPilotTask, Local, COND_None);
}

FTGOR_MovementPosition UTGOR_AttachedPilotTask::ComputePosition() const
{
	return Local.BaseToPosition(ComputeParentPosition());
}

FTGOR_MovementSpace UTGOR_AttachedPilotTask::ComputeSpace() const
{
	return Local.BaseToSpace(ComputeParentSpace());
}

void UTGOR_AttachedPilotTask::InitDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.PositionToBase(ParentDynamic, Dynamic);
	Super::InitDynamic(Dynamic);
}

void UTGOR_AttachedPilotTask::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.PositionToBase(ParentDynamic, Dynamic);
	Super::SimulatePosition(Dynamic);
}

void UTGOR_AttachedPilotTask::SimulatePosition(const FTGOR_MovementPosition& Position)
{
	const FTGOR_MovementPosition ParentPosition = ComputeParentPosition();
	Local.PositionToBase(ParentPosition, Position);
	Super::SimulatePosition(Position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AttachedPilotTask::RepNotifyLocal(const FTGOR_MovementPosition& Old)
{
	if (IsValid(Identifier.Component))
	{
		ENetRole Role = Identifier.Component->GetOwnerRole();
		if (Role == ENetRole::ROLE_AutonomousProxy)
		{
			if (Old.ComparePosition(Local, Identifier.Component->AdjustThreshold) < 1.0f)
			{
				Local = Old;
			}
		}
	}
}
