// The Gateway of Realities: Planes of Existence.

#include "TGOR_Craft.h"
#include "InventorySystem/Tasks/TGOR_ProcessTask.h"
#include "InventorySystem/Components/TGOR_ProcessComponent.h"
#include "InventorySystem/Components/TGOR_StorageComponent.h"
#include "InventorySystem/Components/TGOR_MatterComponent.h"

UTGOR_Craft::UTGOR_Craft()
	: Super()
{
}

/*
float UTGOR_Craft::Process(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage, float Antimatter)
{
	Super::Process(Owner, Storage, Antimatter);
	UTGOR_StorageComponent* Sockets = Owner->GetOwnerComponent<UTGOR_StorageComponent>();
	UTGOR_MatterComponent* Matter = Owner->GetOwnerComponent<UTGOR_MatterComponent>();
	return OnProcess(Sockets, Matter, Storage, Antimatter);
}

bool UTGOR_Craft::CanProcess(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage) const
{
	if (Super::CanProcess(Owner, Storage))
	{
		UTGOR_StorageComponent* Sockets = Owner->GetOwnerComponent<UTGOR_StorageComponent>();
		UTGOR_MatterComponent* Matter = Owner->GetOwnerComponent<UTGOR_MatterComponent>();
		if (IsValid(Sockets) && IsValid(Matter))
		{
			return OnCanProcess(Sockets, Matter, Storage);
		}
	}
	return false;
}
*/
