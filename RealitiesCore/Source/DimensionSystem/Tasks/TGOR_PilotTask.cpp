// The Gateway of Realities: Planes of Existence.

#include "TGOR_PilotTask.h"
#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"


UTGOR_PilotTask::UTGOR_PilotTask()
: Super(),
Influence()
{
}

void UTGOR_PilotTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_PilotTask, Identifier, COND_InitialOnly);
}

void UTGOR_PilotTask::BeginDestroy()
{
	// Clear parent in case we get killed without having proper unparented before
	Unparent();

	Super::BeginDestroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Pilot* UTGOR_PilotTask::GetPilot() const
{
	return Identifier.Content;
}


UTGOR_PilotComponent* UTGOR_PilotTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_PilotTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

void UTGOR_PilotTask::Update(float DeltaTime)
{

}

void UTGOR_PilotTask::Unparent()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MobilityComponent* UTGOR_PilotTask::GetParent() const
{
	return nullptr;
}

FTGOR_MovementDynamic UTGOR_PilotTask::ComputeBase() const
{
	UTGOR_MobilityComponent* Parent = GetParent();
	if (IsValid(Parent))
	{
		return Parent->ComputeSpace();
	}
	return FTGOR_MovementDynamic();
}

FTGOR_MovementPosition UTGOR_PilotTask::ComputePosition() const
{
	return ComputeSpace();
}

FTGOR_MovementSpace UTGOR_PilotTask::ComputeSpace() const
{
	return ComputeBase();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_PilotTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);
	OnInitialise();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PilotTask::Register()
{
	IsRegisteredToParent = true;
}

void UTGOR_PilotTask::Unregister()
{
	IsRegisteredToParent = false;
}

bool UTGOR_PilotTask::IsRegistered() const
{
	return IsRegisteredToParent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PilotTask::SetInfluence(const FTGOR_MovementForce& Force)
{
	Influence = Force;
}

const FTGOR_MovementForce& UTGOR_PilotTask::GetInfluence() const
{
	return Influence;
}
