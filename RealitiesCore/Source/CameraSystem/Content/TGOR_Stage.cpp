// The Gateway of Realities: Planes of Existence.

#include "TGOR_Stage.h"

#include "CameraSystem/Content/TGOR_Camera.h"


UTGOR_Stage::UTGOR_Stage()
	: Super()
{
}

void UTGOR_Stage::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(CameraInsertions);
}
