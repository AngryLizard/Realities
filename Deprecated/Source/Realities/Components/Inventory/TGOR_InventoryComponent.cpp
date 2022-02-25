// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventoryComponent.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"


UTGOR_InventoryComponent::UTGOR_InventoryComponent()
	: Super(),
	MaxContainerCount(5),
	MaxItemCount(16)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}

void UTGOR_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTGOR_InventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


bool UTGOR_InventoryComponent::CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const
{
	if (Super::CanStoreItemAt(Index, Storage))
	{
		return true;
	}
	return false;
}

int32 UTGOR_InventoryComponent::GetContainerCapacity(const FTGOR_ContainerIndex& Index) const
{
	if (Index.IsValid() && 0 <= Index.Container && Index.Container < MaxContainerCount)
	{
		return MaxItemCount;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemStorage* UTGOR_InventoryComponent::PutItem(UTGOR_ItemStorage* Storage)
{
	if (IsValid(Storage))
	{
		// Look for empty container that supports given item
		for (int32 Container = 0; Container < MaxContainerCount; Container++)
		{
			Storage = PutItemInto(FTGOR_ContainerIndex(Container), Storage);
		}
	}
	return Storage;
}

UTGOR_ItemStorage* UTGOR_InventoryComponent::PutItemInto(const FTGOR_ContainerIndex& Index, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Storage))
	{
		// Look for empty slot that supports given item
		TArray<UTGOR_ItemStorage*> Storages = GetItemSlots(Index);
		for (int32 Slot = 0; Slot < Storages.Num(); Slot++)
		{
			// SwapItemStorage does validity checks for us
			FTGOR_SlotIndex SlotIndex(Index, Slot);
			if (!IsValid(Storages[Slot]) && CanStoreItemAt(SlotIndex, Storage))
			{
				UTGOR_ItemStorage* Residual = SwapItemStorage(SlotIndex, Storage);
				OnItemPut.Broadcast(Storage);
				return Residual;
			}
		}
	}
	return Storage;
}

UTGOR_ItemStorage* UTGOR_InventoryComponent::CreateItem(TSubclassOf<UTGOR_Item> Type)
{
	SINGLETON_RETCHK(nullptr);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Item* Item = ContentManager->GetTFromType(Type);
	if (IsValid(Item))
	{
		UTGOR_ItemStorage* Storage = Item->CreateStorage();
		if (IsValid(Storage))
		{
			return PutItem(Storage);
		}
	}
	return nullptr;
}


