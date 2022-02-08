// The Gateway of Realities: Planes of Existence.
#include "TGOR_AttachedComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Tasks/TGOR_AttachedPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "DrawDebugHelpers.h"

UTGOR_AttachedComponent::UTGOR_AttachedComponent()
:	Super()
{
}

bool UTGOR_AttachedComponent::ParentAttached(UTGOR_PilotComponent* Attachee, int32 Index, const FTGOR_MovementPosition& Position)
{
	if (IsValid(Attachee))
	{
		TArray<UTGOR_AttachedPilotTask*> Tasks = Attachee->GetPListOfType<UTGOR_AttachedPilotTask>();
		for (UTGOR_AttachedPilotTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Index))
			{
				Task->Parent(this, Index);

				// Set location without triggering any movement events (prevent infinite loops when parent volume changes)
				Task->InitDynamic(Position);

				Attachee->AttachWith(Task->Identifier.Slot);

				// Trigger movement events now
				Attachee->OnPositionChange(Position);
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_AttachedComponent::CanParentAttached(UTGOR_PilotComponent* Attachee, int32 Index) const
{
	if (IsValid(Attachee))
	{
		TArray<UTGOR_AttachedPilotTask*> Tasks = Attachee->GetPListOfType<UTGOR_AttachedPilotTask>();
		for (UTGOR_AttachedPilotTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Index))
			{
				return true;
			}
		}
	}
	return false;
}
