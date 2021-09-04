// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Resource.h"

#include "InventorySystem/Content/TGOR_Segment.h"


UTGOR_Resource::UTGOR_Resource()
	: Super()
{
}


void UTGOR_Resource::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(SegmentInsertions);
}

