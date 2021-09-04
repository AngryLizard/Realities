// The Gateway of Realities: Planes of Existence.

#include "TGOR_Process.h"
#include "InventorySystem/Storage/TGOR_ProcessStorage.h"
#include "InventorySystem/Components/TGOR_ProcessComponent.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"

UTGOR_Process::UTGOR_Process()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ProcessStorage* UTGOR_Process::CreateStorage()
{
	UTGOR_ProcessStorage* Storage = NewObject<UTGOR_ProcessStorage>(GetTransientPackage());
	Storage->ProcessRef = this;
	BuildStorage(Storage);
	return Storage;
}

void UTGOR_Process::BuildStorage(UTGOR_ProcessStorage* Storage)
{
	Storage->BuildModules(Modules);
	InitialiseStorage(Storage);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UTGOR_Process::Tick(UTGOR_DimensionComponent* Owner, UTGOR_ProcessStorage* Storage, float DeltaSeconds)
{
}

float UTGOR_Process::Process(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage, float Antimatter)
{
	return Antimatter;
}

bool UTGOR_Process::CanProcess(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage) const
{
	return IsValid(Owner);
}