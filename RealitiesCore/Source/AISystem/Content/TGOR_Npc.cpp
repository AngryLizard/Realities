// The Gateway of Realities: Planes of Existence.


#include "TGOR_Npc.h"

#include "CreatureSystem/Content/TGOR_Creature.h"
#include "AISystem/Actors/TGOR_NpcSpawnActor.h"
#include "AISystem/Gameplay/TGOR_NpcController.h"

UTGOR_Npc::UTGOR_Npc()
	: Super()
{
	Controller = ATGOR_NpcController::StaticClass();
}

bool UTGOR_Npc::Validate_Implementation()
{
	if (!*Spawner)
	{
		ERRET("No spawner defined", Error, false);
	}

	if (!*Controller)
	{
		ERRET("No controller defined", Error, false);
	}

	return Super::Validate_Implementation();
}

TSubclassOf<AActor> UTGOR_Npc::GetSpawnClass_Implementation() const
{
	return Spawner;
}

void UTGOR_Npc::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(CreatureInsertion);
}

