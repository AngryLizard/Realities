// The Gateway of Realities: Planes of Existence.

#include "TGOR_Section.h"

#include "CustomisationSystem/Content/TGOR_Canvas.h"

UTGOR_Section::UTGOR_Section()
	: Super(),
	Material(nullptr),
	TransformUV(true)
{
}

void UTGOR_Section::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(CanvasInsertions);
}

