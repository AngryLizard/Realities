// The Gateway of Realities: Planes of Existence.


#include "TGOR_Modular.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"

UTGOR_Modular::UTGOR_Modular()
	: Super(),
	Skeleton(nullptr)
{
}

void UTGOR_Modular::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(BodypartInsertions);
}

