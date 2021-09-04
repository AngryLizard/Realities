// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Entity.h"

#include "TGOR_Target.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

UTGOR_Entity::UTGOR_Entity()
	: Super()
{
}

void UTGOR_Entity::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(AttributeInsertions);
	MOV_INSERTION(TargetInsertions);
}
