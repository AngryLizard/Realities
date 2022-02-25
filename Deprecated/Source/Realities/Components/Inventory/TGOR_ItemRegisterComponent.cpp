// The Gateway of Realities: Planes of Existence.

#include "TGOR_ItemRegisterComponent.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Actors/Inventory/TGOR_ItemDropActor.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"

#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ItemContainers)

void FTGOR_ItemContainers::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Slots", Slots);
}

void FTGOR_ItemContainers::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Slots", Slots);
}

void FTGOR_ItemContainers::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Slots);
}

void FTGOR_ItemContainers::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Slots);
}




/*
bool FTGOR_ItemContainers::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const ENetRole Role = DeltaParms.Connection->OwningActor->Role;

	VPORT(Role);
	// 1 ROLE_SimulatedProxy
	// 2 ROLE_AutonomousProxy
	// 3 ROLE_Authority

	//return FFastArraySerializer::FastArrayDeltaSerialize<FTGOR_ItemContainer, FTGOR_ItemContainers>(Containers, DeltaParms, *this);

	return true;
}
*/



UTGOR_ItemRegisterComponent::UTGOR_ItemRegisterComponent()
	: Super()
{
}

void UTGOR_ItemRegisterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasAccess())
	{
		TickAll(DeltaTime);
	}
}

void UTGOR_ItemRegisterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ItemRegisterComponent, ItemContainers, COND_OwnerOnly);
}

TSet<UTGOR_Content*> UTGOR_ItemRegisterComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ActiveContent;

	// TODO: Do we want to track all items? The possibilities are pretty nice but need to test what this does to the tracker system

	return ActiveContent;
}


void UTGOR_ItemRegisterComponent::TickAll(float DeltaTime)
{
	// Tick all slots (reverse order in case of purge)
	for (auto It = ItemContainers.Slots.CreateIterator(); It; ++It)
	{
		if (IsValid(It->Value.Storage))
		{
			It->Value.Storage->Tick(this, DeltaTime);
			if (It->Value.Storage->PurgeDirt())
			{
				// TODO: Use this for deltaNetSerialize
				ContainerUpdate();
			}
		}
		else
		{
			// Clean slots from empty items
			It.RemoveCurrent();
		}
	}

	// TODO: On simulated client, currently active items aren't ticked. Yay/Nay?
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ItemRegisterComponent::ForEachSlot(const FSlotDelegate& Delegate, int32 Compare)
{
	int32 Count = 0;
	ForEachSlot([&Count, Delegate, Compare](const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Item)->bool
	{
		Delegate.ExecuteIfBound(Index, Item, Count++, Compare);
		return true;
	});
}

bool UTGOR_ItemRegisterComponent::ForEachSlot(std::function<bool(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Item)> func)
{
	for (const auto& Pair : ItemContainers.Slots)
	{
		if (IsValid(Pair.Value.Storage))
		{
			// Call function with computed index
			if (!func(Pair.Key, Pair.Value.Storage))
			{
				return false;
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemRegisterComponent::RepNotifyItemContainers()
{
	ContainerUpdate();
}

UTGOR_ItemStorage* UTGOR_ItemRegisterComponent::GetItemStorageChecked(const FTGOR_SlotIndex& Index, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	UTGOR_ItemStorage* Storage = GetItemStorage(Index);
	if (IsValid(Storage))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return Storage;
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_ItemRegisterComponent::GetItemStorage(const FTGOR_SlotIndex& Index) const
{
	const FTGOR_ItemInstance* Ptr = ItemContainers.Slots.Find(Index);
	if (Ptr)
	{
		return Ptr->Storage;
	}
	return nullptr;
}

TArray<UTGOR_ItemStorage*> UTGOR_ItemRegisterComponent::GetItemSlots(const FTGOR_ContainerIndex& Index) const
{
	TArray<UTGOR_ItemStorage*> Slots;

	const int32 Capacity = GetContainerCapacity(Index);
	if (Capacity > 0)
	{
		Slots.SetNumZeroed(Capacity);
		for (const auto& Pair : ItemContainers.Slots)
		{
			if (Pair.Key == Index)
			{
				int32 Slot = (int32)Pair.Key.Slot;
				if (Slots.IsValidIndex(Slot))
				{
					Slots[Slot] = Pair.Value.Storage;
				}
			}
		}
	}
	return Slots;
}

UTGOR_ItemStorage* UTGOR_ItemRegisterComponent::SwapItemStorage(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage)
{
	if (!CanStoreItemAt(Index, Storage))
	{
		return Storage;
	}

	FTGOR_ItemInstance* Ptr = ItemContainers.Slots.Find(Index);
	if (Ptr)
	{
		std::swap(Ptr->Storage, Storage);
		ContainerUpdate();
		return Storage;
	}

	if (IsValid(Storage))
	{
		ItemContainers.Slots.Emplace(Index, FTGOR_ItemInstance(Storage));
		ContainerUpdate();
	}
	return nullptr;
}

void UTGOR_ItemRegisterComponent::OverrideItemStorage(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage)
{
	FTGOR_ItemInstance* Ptr = ItemContainers.Slots.Find(Index);
	if (Ptr)
	{
		Ptr->Storage = Storage;
	}
	else if (IsValid(Storage))
	{
		ItemContainers.Slots.Emplace(Index, FTGOR_ItemInstance(Storage));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ItemRegisterComponent::CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const
{
	const int32 Capacity = GetContainerCapacity(Index);
	return Index.Valid && 0 <= Index.Slot && Index.Slot < Capacity;
}

int32 UTGOR_ItemRegisterComponent::GetContainerCapacity(const FTGOR_ContainerIndex& Index) const
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ItemRegisterComponent::HasAccess() const
{
	return GetOwnerRole() >= ENetRole::ROLE_AutonomousProxy;
}


void UTGOR_ItemRegisterComponent::AccessBranch(ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;
	if (HasAccess())
	{
		Branches = ETGOR_BoolEnumeration::Is;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ItemRegisterComponent::PurgeItemStorage(UTGOR_ItemStorage* Storage)
{
	bool PurgedAny = false;
	for (auto& Pair : ItemContainers.Slots)
	{
		if (Pair.Value.Storage == Storage)
		{
			// We don't remove the entry itself since this can be called from within a loop
			Pair.Value.Storage = nullptr;
			ContainerUpdate();
			PurgedAny = true;
		}
		else if (IsValid(Pair.Value.Storage))
		{
			PurgedAny = Pair.Value.Storage->Purge(Storage) || PurgedAny;
		}
	}
	return PurgedAny;
}