
#include "TGOR_ItemComponent.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"

UTGOR_ItemComponent::UTGOR_ItemComponent()
	: Super()
{
}

void UTGOR_ItemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_ItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ItemSlot.Update(this, DeltaTime))
	{
		// TODO: Use this for deltaNetSerialize
		ContainerUpdate();
	}
}

void UTGOR_ItemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ItemComponent, ItemSlot, COND_None);
}

TSet<UTGOR_CoreContent*> UTGOR_ItemComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> Active = Super::GetActiveContent();
	if (IsValid(ItemSlot.Storage))
	{
		Active.Emplace(ItemSlot.Storage->GetItem());
	}
	return Active;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ItemComponent::PurgeItemStorage(UTGOR_ItemStorage* Storage)
{
	if (ItemSlot.Storage == Storage)
	{
		// We don't remove the entry itself since this can be called from within a loop
		ItemSlot.Storage = nullptr;
		ContainerUpdate();
		return true;
	}
	else if (IsValid(ItemSlot.Storage))
	{
		return ItemSlot.Storage->Purge(Storage);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemComponent::RepNotifyItemSlot()
{
	ContainerUpdate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemStorage* UTGOR_ItemComponent::PutItem(UTGOR_ItemStorage* Storage)
{
	// Unpack first
	OnUnpack.Broadcast(Storage);

	// Store item into inventory
	UTGOR_ItemStorage* Residual = ItemSlot.Storage;
	ItemSlot.Storage = Storage;
	ContainerUpdate();
	return Residual;
}

UTGOR_ItemStorage* UTGOR_ItemComponent::PickItem()
{
	// Take item from inventory
	UTGOR_ItemStorage* Storage = ItemSlot.Storage;
	ItemSlot.Storage = nullptr;

	// Pack first
	OnPack.Broadcast(Storage);

	// Despawn ItemDrop after removing item
	UTGOR_PilotComponent* Pilot = GetOwnerComponent<UTGOR_PilotComponent>();
	if (IsValid(Pilot))
	{
		Pilot->AttachWith(INDEX_NONE);
	}

	UTGOR_GameInstance::DespawnActor(GetOwner());
	return Storage;
}

UTGOR_ItemStorage* UTGOR_ItemComponent::PeekStorage() const
{
	return ItemSlot.Storage;
}