// The Gateway of Realities: Planes of Existence.

#include "TGOR_ActionGroundTask.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "ActionSystem/Components/TGOR_ActionComponent.h"
#include "ActionSystem/Tasks/TGOR_ActionTask.h"

UTGOR_ActionGroundTask::UTGOR_ActionGroundTask()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionGroundTask::Initialise()
{
	Super::Initialise();

	ActionComponent = RootComponent->GetOwnerComponent<UTGOR_ActionComponent>();
}

bool UTGOR_ActionGroundTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External) || !ActionComponent.IsValid())
	{
		return false;
	}

	return ActionComponent->CanRunMovement(Identifier.Content);
}

FTGOR_MovementPosition UTGOR_ActionGroundTask::TickAnimationRootMotion(FTGOR_MovementSpace& Space, float DeltaTime)
{
	const FTGOR_MovementPosition Offset = Super::TickAnimationRootMotion(Space, DeltaTime);

	UTGOR_ActionTask* ActionTask = ActionComponent->GetCurrentAOfType<UTGOR_ActionTask>();
	if (IsValid(ActionTask))
	{
		// Rebasing is the same as combining
		// TODO: This assumes that action root motion always happens after movement, which should always be the case (and usually there won't be movement and action rootmotion at the same time anyway).
		//       But keep in mind that this would quite significantly break if the order of the two happens to be different.
		return Offset.BaseToPosition(ActionTask->TickAnimationRootMotion(Space, DeltaTime));
	}
	return Offset;
}

float UTGOR_ActionGroundTask::GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return TargetStretch;
}