// The Gateway of Realities: Planes of Existence.
#include "TGOR_Stat.h"
#include "SimulationSystem/Content/TGOR_Response.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

UTGOR_Stat::UTGOR_Stat()
	: Super(),
	Type(ETGOR_StatTypeEnumeration::Hidden),
	Initial(0.0f),
	Duration(0.5f)
{
}

void UTGOR_Stat::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ResponseInsertions);
	MOV_INSERTION(AttributeInsertions);
}
