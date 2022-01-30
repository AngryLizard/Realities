// The Gateway of Realities: Planes of Existence.


#include "TGOR_EuclideanPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_EuclideanPilotTask::UTGOR_EuclideanPilotTask()
{
}

void UTGOR_EuclideanPilotTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_EuclideanPilotTask, Local, COND_None);
}

FTGOR_MovementPosition UTGOR_EuclideanPilotTask::ComputePosition() const
{
	return Local.BaseToPosition(ComputeParentPosition());
}

FTGOR_MovementSpace UTGOR_EuclideanPilotTask::ComputeSpace() const
{
	return Local.BaseToSpace(ComputeParentSpace());
}

void UTGOR_EuclideanPilotTask::InitDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.DynamicToBase(ParentDynamic, Dynamic);
	Super::InitDynamic(Dynamic);
}

void UTGOR_EuclideanPilotTask::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.DynamicToBase(ParentDynamic, Dynamic);
	Super::SimulatePosition(Dynamic);
}

void UTGOR_EuclideanPilotTask::SimulatePosition(const FTGOR_MovementPosition& Position)
{
	const FTGOR_MovementPosition ParentPosition = ComputeParentPosition();
	Local.PositionToBase(ParentPosition, Position);
	Super::SimulatePosition(Position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EuclideanPilotTask::ResetToComponent(UTGOR_MobilityComponent* Owner)
{
	const FTGOR_MovementPosition ParentPosition = ComputeParentPosition();
	Local.PositionToBase(ParentPosition, Owner->GetComponentPosition());
}

void UTGOR_EuclideanPilotTask::RepNotifyLocal(const FTGOR_MovementDynamic& Old)
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
