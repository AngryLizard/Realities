// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpawnerInterface.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DimensionSystem/Content/TGOR_SpawnModule.h"

void FTGOR_SpawnerDependencies::Process(TSubclassOf<UObject> Class)
{
	if (!IsValid(Spawner))
	{
		return;
	}

	int32 Index = Pointer;
	while (Objects.IsValidIndex(Index))
	{
		UObject* Object = Objects[Index];
		UClass* ObjectClass = Object->GetClass();
		if (ObjectClass && (ObjectClass->IsChildOf(Class) || ObjectClass->ImplementsInterface(Class)))
		{
			// Swap to end of list
			if (Index != Pointer)
			{
				Swap(Object, Objects[Pointer]);
			}

			// Continue pointer
			UObject* Target = Objects[Pointer];
			Pointer -= 1;

			// Actually process dependency
			ITGOR_SpawnerInterface::Execute_UpdateContent(Target, *this);

		}

		// Go to next available entry
		Index = FMath::Min(Index - 1, Pointer);
	}
}

void ITGOR_SpawnerInterface::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
}

/*
TSubclassOf<UTGOR_SpawnModule> ITGOR_SpawnerInterface::GetModuleType_Implementation() const
{
	return UTGOR_SpawnModule::StaticClass();
}
*/