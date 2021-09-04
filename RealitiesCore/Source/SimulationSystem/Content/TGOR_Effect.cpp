// The Gateway of Realities: Planes of Existence.

#include "TGOR_Effect.h"

#include "AttributeSystem/Content/TGOR_Attribute.h"


UTGOR_Effect::UTGOR_Effect()
:	Super(),
	Min(0.0f),
	Max(1.0f)
{

}

void UTGOR_Effect::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(AttributeInsertions);
}
