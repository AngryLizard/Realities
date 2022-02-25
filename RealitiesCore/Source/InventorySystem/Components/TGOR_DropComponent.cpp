// The Gateway of Realities: Planes of Existence.

#include "TGOR_DropComponent.h"

#include "Net/UnrealNetwork.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/GamePlay/TGOR_GameInstance.h"
#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"


UTGOR_DropComponent::UTGOR_DropComponent()
	: Super()
{
}

UTGOR_ItemTask* UTGOR_DropComponent::PushItem(UTGOR_ItemTask* Task)
{
	UTGOR_ItemTask* Previous = nullptr;
	if (CurrentItem.IsValid())
	{
		CurrentItem->AssignInventory(nullptr);

		Previous = CurrentItem.Get();
		CurrentItem.Reset();
	}

	CurrentItem = Task;
	if (CurrentItem.IsValid())
	{
		CurrentItem->AssignInventory(this);
	}
	else
	{
		UTGOR_GameInstance::DespawnActor(GetOwner());
	}
	return Previous;
}

UTGOR_ItemTask* UTGOR_DropComponent::PullItem()
{
	if (CurrentItem.IsValid())
	{
		CurrentItem->AssignInventory(nullptr);

		UTGOR_ItemTask* Previous = CurrentItem.Get();
		CurrentItem.Reset();

		UTGOR_GameInstance::DespawnActor(GetOwner());
		return Previous;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemTask* UTGOR_DropComponent::GetCurrentItem() const
{
	return CurrentItem.Get();
}
