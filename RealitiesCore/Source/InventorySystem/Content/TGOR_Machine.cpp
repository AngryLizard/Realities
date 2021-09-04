// The Gateway of Realities: Planes of Existence.


#include "TGOR_Machine.h"

#include "InventorySystem/Actors/TGOR_MachineActor.h"

UTGOR_Machine::UTGOR_Machine()
	: Super()
{
}

TSubclassOf<AActor> UTGOR_Machine::GetSpawnClass_Implementation() const
{
	return Machine;
}
