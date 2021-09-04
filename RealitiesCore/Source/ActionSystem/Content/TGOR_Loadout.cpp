// The Gateway of Realities: Planes of Existence.

#include "TGOR_Loadout.h"

#include "ActionSystem/Content/TGOR_Action.h"
#include "MovementSystem/Content/TGOR_Movement.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"


FTGOR_ActionSlotSpot::FTGOR_ActionSlotSpot()
	: Obligatory(false)
{
}


UTGOR_Loadout::UTGOR_Loadout()
	: Super()
{
}


void UTGOR_Loadout::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(StaticActionInsertions);
	MOV_INSERTION(SlotActionInsertions);
}
