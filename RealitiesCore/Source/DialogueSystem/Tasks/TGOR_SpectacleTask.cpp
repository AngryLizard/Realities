// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpectacleTask.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "DialogueSystem/Content/TGOR_Participant.h"
#include "DialogueSystem/Content/TGOR_Spectacle.h"
#include "DialogueSystem/Components/TGOR_SpectacleComponent.h"

UTGOR_SpectacleTask::UTGOR_SpectacleTask()
: Super()
{
}

bool UTGOR_SpectacleTask::CanStart(const FTGOR_SpectacleConfiguration& Configuration) const
{
	// Make sure configuration is valid
	for (const auto& Pair : Participants)
	{
		TWeakObjectPtr<AActor> const* Ptr = Configuration.Participants.Find(Pair.Key);
		if (!Ptr || !Ptr->IsValid())
		{
			return false;
		}

		if (!Pair.Value->IsAvailable() || !Pair.Value->IsCompatibleWith(Ptr->Get()))
		{
			return false;
		}
	}

	ETGOR_ValidEnumeration State = ETGOR_ValidEnumeration::Valid;
	OnCondition(State);
	return State == ETGOR_ValidEnumeration::Valid;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SpectacleTask::Initialise()
{
	OnInitialise();
	return true;
}

void UTGOR_SpectacleTask::Prepare(FTGOR_SpectacleState& DialogueState)
{
	PlayAnimation();
}

bool UTGOR_SpectacleTask::Update(FTGOR_SpectacleState& DialogueState, float DeltaTime)
{
	return true;
}

void UTGOR_SpectacleTask::Interrupt(FTGOR_SpectacleState& DialogueState)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SpectacleTask::ComputeCompatibleConfiguration(const TArray<AActor*>& Actors, FTGOR_SpectacleConfiguration& Configuration) const
{
	// TODO: Make this faster
	// TODO: This could fail even though a different actor order would work
	// Spread actors across all participants
	TSet<AActor*> Availables = TSet<AActor*>(Actors);
	for (const auto& Pair : Participants)
	{
		// Assign linked actors
		if (Pair.Value->HasLinkedActor())
		{
			Configuration.Participants.Emplace(Pair.Key, Pair.Value->GetLinkedActor());
			continue;
		}

		// Check all proposed actors for a match
		for (AActor* Actor : Actors)
		{
			if (Pair.Value->IsAvailable() && Pair.Value->IsCompatibleWith(Actor))
			{
				Configuration.Participants.Emplace(Pair.Key, Actor);
				break;
			}
		}

		// Check whether a participant has been found
		if (TWeakObjectPtr<AActor>* Ptr = Configuration.Participants.Find(Pair.Key))
		{
			Availables.Remove(Ptr->Get());
		}
		else
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SpectacleTask::InitialiseParticipants(UTGOR_SpectacleComponent* Owner, UTGOR_Spectacle* Spectacle)
{
	// Gather all participant components
	Participants.Reset();
	for (UTGOR_Participant* Participant : Spectacle->Instanced_ParticipantInsertions.Collection)
	{
		UActorComponent* ParticipantComponent = Owner->GetParticipantComponent(Participant);
		if (!IsValid(ParticipantComponent))
		{
			EXCPT(FString::Printf(TEXT("No participant component found for %s"), *Participant->GetName()));
			return false;
		}

		Participants.Emplace(Participant, ParticipantComponent);
	}

	return true;
}