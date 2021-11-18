// The Gateway of Realities: Planes of Existence.

#include "TGOR_Creature.h"

#include "KnowledgeSystem/Content/TGOR_Tracker.h"


UTGOR_Creature::UTGOR_Creature()
	: Super()
{
}

bool UTGOR_Creature::Validate_Implementation()
{
	if (!*Pawn)
	{
		ERRET("No pawn defined", Error, false);
	}

	return Super::Validate_Implementation();
}

TSubclassOf<AActor> UTGOR_Creature::GetSpawnClass_Implementation() const
{
	return Pawn;
}

void UTGOR_Creature::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(TrackerInsertions);
}
