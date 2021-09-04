// The Gateway of Realities: Planes of Existence.


#include "TGOR_Poser.h"

#include "AnimationSystem/Content/TGOR_Archetype.h"

UTGOR_Poser::UTGOR_Poser()
	: Super()
{
}

void UTGOR_Poser::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ArchetypeInsertions);
}

