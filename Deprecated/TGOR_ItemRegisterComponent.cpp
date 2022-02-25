// The Gateway of Realities: Planes of Existence.

#include "TGOR_ItemRegisterComponent.h"

#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Content/TGOR_Item.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "DimensionSystem/Components/TGOR_MobilityComponent.h"

#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"

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

TSet<UTGOR_CoreContent*> UTGOR_ItemRegisterComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> ActiveContent;

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
			if (It->Value.Update(this, DeltaTime))
			{
				ContainerUpdate();
			}
			It->Value.Post();
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
	ForEachSlot([&Count, Delegate, Compare](int32 Identifier, UTGOR_ItemStorage* Item)->bool
	{
		Delegate.ExecuteIfBound(Identifier, Item, Count++, Compare);
		return true;
	});
}

bool UTGOR_ItemRegisterComponent::ForEachSlot(std::function<bool(int32 Identifier, UTGOR_ItemStorage* Item)> func)
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

UTGOR_ItemStorage* UTGOR_ItemRegisterComponent::GetItemStorageValidated(int32 Identifier, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	UTGOR_ItemStorage* Storage = GetItemStorage(Identifier);
	if (IsValid(Storage))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return Storage;
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_ItemRegisterComponent::GetItemStorage(int32 Identifier) const
{
	const FTGOR_ItemInstance* Ptr = ItemContainers.Slots.Find(Identifier);
	if (Ptr)
	{
		return Ptr->Storage;
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_ItemRegisterComponent::SwapItemStorage(int32 Identifier, UTGOR_ItemStorage* Storage)
{
	if (!CanStoreItemAt(Identifier, Storage))
	{
		return Storage;
	}

	FTGOR_ItemInstance* Ptr = ItemContainers.Slots.Find(Identifier);
	if (Ptr)
	{
		std::swap(Ptr->Storage, Storage);
		ContainerUpdate();
		return Storage;
	}

	if (IsValid(Storage))
	{
		ItemContainers.Slots.Emplace(Identifier, FTGOR_ItemInstance(Storage));
		ContainerUpdate();
	}
	return nullptr;
}

void UTGOR_ItemRegisterComponent::OverrideItemStorage(int32 Identifier, UTGOR_ItemStorage* Storage)
{
	FTGOR_ItemInstance* Ptr = ItemContainers.Slots.Find(Identifier);
	if (Ptr)
	{
		Ptr->Storage = Storage;
	}
	else if (IsValid(Storage))
	{
		ItemContainers.Slots.Emplace(Identifier, FTGOR_ItemInstance(Storage));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ItemRegisterComponent::CanStoreItemAt(int32 Identifier, UTGOR_ItemStorage* Storage) const
{
	return Identifier >= 0;
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