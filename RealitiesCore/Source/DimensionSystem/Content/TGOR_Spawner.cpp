// The Gateway of Realities: Planes of Existence.


#include "TGOR_Spawner.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Content/TGOR_SpawnModule.h"

UTGOR_Spawner::UTGOR_Spawner()
	: Super()
{
}

bool UTGOR_Spawner::Validate_Implementation()
{
	if (!*GetSpawnClass())
	{
		ERRET("No spawn type defined", Error, false);
	}

	return Super::Validate_Implementation();
}

TSubclassOf<AActor> UTGOR_Spawner::GetSpawnClass_Implementation() const
{
	return AActor::StaticClass();
}

UTGOR_Content* UTGOR_Spawner::GetModuleFromType(TSubclassOf<UTGOR_SpawnModule> Type) const
{
	SINGLETON_RETCHK(nullptr);

	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	return ContentManager->GetTFromType<UTGOR_SpawnModule>(Type);
}

/**
void UTGOR_Spawner::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ModuleInsertions);
}
*/
