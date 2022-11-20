// The Gateway of Realities: Planes of Existence.

#include "TGOR_ParticipantInterface.h"
#include "DialogueSystem/Content/TGOR_Participant.h"

bool ITGOR_ParticipantInterface::IsCompatibleWith(AActor* Actor) const
{
	return false;
}

UTGOR_Participant* ITGOR_ParticipantInterface::GetParticipant() const
{
	return nullptr;
}

AActor* ITGOR_ParticipantInterface::GetLinkedActor() const
{
	return nullptr;
}

bool ITGOR_ParticipantInterface::HasLinkedActor() const
{
	return false;
}

bool ITGOR_ParticipantInterface::IsAvailable() const
{
	if (!IsValid(GetParticipant()))
	{
		return false;
	}

	return !HasLinkedActor() || IsValid(GetLinkedActor());
}