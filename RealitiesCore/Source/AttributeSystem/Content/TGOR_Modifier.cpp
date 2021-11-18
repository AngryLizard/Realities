// The Gateway of Realities: Planes of Existence.
#include "TGOR_Modifier.h"

#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_Modifier::UTGOR_Modifier()
	: Super()
{
}

void UTGOR_Modifier::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(AttributeInsertions);
}
