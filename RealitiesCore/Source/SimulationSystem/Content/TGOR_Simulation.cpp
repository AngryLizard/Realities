// The Gateway of Realities: Planes of Existence.
#include "TGOR_Simulation.h"

#include "TGOR_Stat.h"

UTGOR_Simulation::UTGOR_Simulation()
	: Super()
{
}

void UTGOR_Simulation::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(StatInsertions);
	MOV_INSERTION(EffectInsertions);
}
