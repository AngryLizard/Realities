// The Gateway of Realities: Planes of Existence.

#include "TGOR_Spectacle.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DialogueSystem/Components/TGOR_ActivatorComponent.h"
#include "DialogueSystem/Components/TGOR_DialogueComponent.h"

UTGOR_Spectacle::UTGOR_Spectacle()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Spectacle::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ParticipantInsertions);
}
