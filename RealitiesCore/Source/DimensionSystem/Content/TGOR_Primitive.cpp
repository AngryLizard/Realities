// The Gateway of Realities: Planes of Existence.

#include "TGOR_Primitive.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"


UTGOR_Primitive::UTGOR_Primitive()
	: Super()
{
}

void UTGOR_Primitive::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(PilotInsertions);
}