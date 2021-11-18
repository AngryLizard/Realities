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
		if (Objects[Index]->IsA(Class))
		{
			// Swap to end of list
			if (Index != Pointer)
			{
				Swap(Objects[Index], Objects[Pointer]);
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