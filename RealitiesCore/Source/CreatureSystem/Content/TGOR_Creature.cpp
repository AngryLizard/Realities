// The Gateway of Realities: Planes of Existence.


#include "TGOR_Creature.h"

#include "AnimationSystem/Content/TGOR_Animation.h"

#include "ActionSystem/Content/TGOR_Loadout.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "KnowledgeSystem/Content/TGOR_Tracker.h"
#include "CreatureSystem/Content/TGOR_Camera.h"


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

	MOV_INSERTION(StomachInsertion);
	MOV_INSERTION(TrackerInsertions);
	MOV_INSERTION(CameraInsertions);
}
