// The Gateway of Realities: Planes of Existence.


#include "TGOR_Participant.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DialogueSystem/Components/TGOR_ActivatorComponent.h"
#include "DialogueSystem/Components/TGOR_DialogueComponent.h"

UTGOR_Participant::UTGOR_Participant()
	: Super()
{
	// By default assume only pawns are able to participate
	ActorFilter = APawn::StaticClass();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Participant::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ConditionInsertions);
}

bool UTGOR_Participant::IsOccupantCompatible(AActor* Actor) const
{
	return Actor && *ActorFilter && Actor->IsA(ActorFilter);
}