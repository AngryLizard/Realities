// The Gateway of Realities: Planes of Existence.

#include "TGOR_Mobile.h"

UTGOR_Mobile::UTGOR_Mobile()
	: Super()
{
}

void UTGOR_Mobile::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(MovementInsertions);
}
