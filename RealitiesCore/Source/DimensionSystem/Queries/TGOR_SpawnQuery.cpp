// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpawnQuery.h"

#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_SpawnQuery::UTGOR_SpawnQuery()
	: Super()
{
}

TArray<UTGOR_CoreContent*> UTGOR_SpawnQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Content;
	Content.Reserve(SpawnObjects.Num());

	// Get attached items
	for (const FTGOR_SpawnModulePair& SpawnObject : SpawnObjects)
	{
		Content.Emplace(SpawnObject.Module);
	}

	return Content;
}


void UTGOR_SpawnQuery::AssignIdentity(UTGOR_IdentityComponent* IdentityComponent)
{
	SpawnObjects.Reset();

	const TArray<UObject*> Objects = IdentityComponent->GetSpawnerObjects();
	for (UObject* Object : Objects)
	{
		FTGOR_SpawnModulePair ModulePair;
		ModulePair.Object = Object;

		const TMap<int32, UTGOR_SpawnModule*> Modules = ITGOR_SpawnerInterface::Execute_GetModuleType(Object);
		for (const auto& Pair : Modules)
		{
			ModulePair.Index = Pair.Key;
			ModulePair.Module = Pair.Value;
			SpawnObjects.Emplace(ModulePair);
		}
	}
}
