// The Gateway of Realities: Planes of Existence.

#include "TGOR_Attachable.h"
#include "SocketSystem/Content/TGOR_Socket.h"

UTGOR_Attachable::UTGOR_Attachable()
	: Super()
{
}

void UTGOR_Attachable::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(SocketInsertions);
}
