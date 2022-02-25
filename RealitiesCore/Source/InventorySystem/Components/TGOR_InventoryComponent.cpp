// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventoryComponent.h"

#include "InventorySystem/Tasks/TGOR_ItemTask.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"


UTGOR_InventoryComponent::UTGOR_InventoryComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}

void UTGOR_InventoryComponent::FreeItem(UTGOR_ItemTask* Task)
{
	if (IsValid(Task))
	{
		Task->FreeToOwner();
	}
}

UTGOR_ItemTask* UTGOR_InventoryComponent::PushItem(UTGOR_ItemTask* Task)
{
	return Task;
}

UTGOR_ItemTask* UTGOR_InventoryComponent::PullItem()
{
	return PushItem(nullptr);
}