// The Gateway of Realities: Planes of Existence.

#include "TGOR_LinkedParticipantComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DialogueSystem/Content/TGOR_Participant.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_LinkedParticipantComponent::UTGOR_LinkedParticipantComponent()
	: Super()
{
}

void UTGOR_LinkedParticipantComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);
	Participant = Dependencies.Spawner->GetMFromType<UTGOR_Participant>(SpawnParticipant);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_LinkedParticipantComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	Modules.Emplace(Modules.Num(), Participant);
	return Modules;
}

bool UTGOR_LinkedParticipantComponent::IsCompatibleWith(AActor* Actor) const
{
	if (!Participant)
	{
		return false;
	}

	if (IsValid(AssignLink) && Actor != AssignLink)
	{
		return false;
	}

	return true;
}

UTGOR_Participant* UTGOR_LinkedParticipantComponent::GetParticipant() const
{
	return Participant;
}

AActor* UTGOR_LinkedParticipantComponent::GetLinkedActor() const
{
	return AssignLink;
}

bool UTGOR_LinkedParticipantComponent::HasLinkedActor() const
{
	return true;
}
