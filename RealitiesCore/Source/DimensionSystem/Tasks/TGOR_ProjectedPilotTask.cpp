// The Gateway of Realities: Planes of Existence.


#include "TGOR_ProjectedPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_ProjectedPilotTask::UTGOR_ProjectedPilotTask()
{
}

void UTGOR_ProjectedPilotTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ProjectedPilotTask, Local, COND_None);
}

FTGOR_MovementPosition UTGOR_ProjectedPilotTask::ComputePosition() const
{
	return Local.BaseToPosition(ComputeParentPosition());
}

FTGOR_MovementSpace UTGOR_ProjectedPilotTask::ComputeSpace() const
{
	return Local.BaseToSpace(ComputeParentSpace());
}

void UTGOR_ProjectedPilotTask::InitDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.DynamicToBase(ParentDynamic, Dynamic);
	Local.Linear.Y = 0.0f;
	Local.LinearVelocity.Y = 0.0f;
	Local.LinearAcceleration.Y = 0.0f;
	Super::InitDynamic(Dynamic);
}

void UTGOR_ProjectedPilotTask::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic ParentDynamic = ComputeParentSpace();
	Local.DynamicToBase(ParentDynamic, Dynamic);
	Local.Linear.Y = 0.0f;
	Local.LinearVelocity.Y = 0.0f;
	Local.LinearAcceleration.Y = 0.0f;
	Super::SimulatePosition(Dynamic);
}

void UTGOR_ProjectedPilotTask::SimulatePosition(const FTGOR_MovementPosition& Position)
{
	const FTGOR_MovementPosition ParentPosition = ComputeParentPosition();
	Local.PositionToBase(ParentPosition, Position);
	Local.Linear.Y = 0.0f;
	Local.LinearVelocity.Y = 0.0f;
	Local.LinearAcceleration.Y = 0.0f;
	Super::SimulatePosition(Position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ProjectedPilotTask::RepNotifyLocal(const FTGOR_MovementDynamic& Old)
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
