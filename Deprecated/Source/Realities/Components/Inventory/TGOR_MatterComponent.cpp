// The Gateway of Realities: Planes of Existence.

#include "TGOR_MatterComponent.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Net/UnrealNetwork.h"


UTGOR_MatterComponent::UTGOR_MatterComponent()
	: Super(),
	MaxMatterCount(4)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	MatterRestriction.ModuleType = UTGOR_MatterModule::StaticClass();
}

void UTGOR_MatterComponent::BeginPlay()
{
	Super::BeginPlay();

	uint8 Slot = 0;
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (TSubclassOf<UTGOR_Item> Container : ContainerSpawn)
	{
		UTGOR_Item* Item = ContentManager->GetTFromType<UTGOR_Item>(Container);
		if (IsValid(Item))
		{
			UTGOR_ItemStorage* Storage = Item->CreateStorage();
			FTGOR_SlotIndex SlotIndex = FTGOR_SlotIndex(0, Slot++);
			UTGOR_ItemStorage* Residual = SwapItemStorage(SlotIndex, Storage);
			DropItemStorage(Residual);
		}
	}
}

void UTGOR_MatterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_MatterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


bool UTGOR_MatterComponent::CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const
{
	if (Super::CanStoreItemAt(Index, Storage))
	{
		if (!IsValid(Storage))
		{
			return true;
		}

		if (Index.Container == 0)
		{
			// Check for type
			UTGOR_Item* Item = Storage->GetItem();
			return Item->IsSupportedBy(MatterRestriction);
		}
		return true;
	}
	return false;
}

int32 UTGOR_MatterComponent::GetContainerCapacity(const FTGOR_ContainerIndex& Index) const
{
	if (!Index.IsValid())
	{
		return 0;
	}

	if (Index.Container == 0)
	{
		return MaxMatterCount;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::GetComposition() const
{
	TArray<UTGOR_ItemStorage*> Slots = GetItemSlots(FTGOR_ContainerIndex(0));

	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Composition;
	for (UTGOR_ItemStorage* Slot : Slots)
	{
		if (IsValid(Slot))
		{
			UTGOR_MatterModule* Module = Slot->GetModule<UTGOR_MatterModule>();
			Module->CombineCompositionOnto(Composition);
		}
	}
	return Composition;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::GetMissing() const
{
	TArray<UTGOR_ItemStorage*> Slots = GetItemSlots(FTGOR_ContainerIndex(0));

	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Missing;
	for (UTGOR_ItemStorage* Slot : Slots)
	{
		if (IsValid(Slot))
		{
			UTGOR_MatterModule* Module = Slot->GetModule<UTGOR_MatterModule>();
			Module->CombineMissingOnto(Missing);
		}
	}
	return Missing;
}

TSet<UTGOR_Matter*> UTGOR_MatterComponent::GetSupportedMatter() const
{
	TArray<UTGOR_ItemStorage*> Slots = GetItemSlots(FTGOR_ContainerIndex(0));

	// Combine all item compositions
	TSet<UTGOR_Matter*> Supported;
	for (UTGOR_ItemStorage* Slot : Slots)
	{
		if (IsValid(Slot))
		{
			UTGOR_MatterModule* Module = Slot->GetModule<UTGOR_MatterModule>();
			Supported.Append(Module->GetSupportedMatter());
		}
	}
	return Supported;
}

int32 UTGOR_MatterComponent::GetStorageCapacity(UTGOR_Matter* Matter) const
{
	TArray<UTGOR_ItemStorage*> Slots = GetItemSlots(FTGOR_ContainerIndex(0));

	// Combine all item compositions
	int32 Capacity = 0;
	for (UTGOR_ItemStorage* Slot : Slots)
	{
		if (IsValid(Slot))
		{
			UTGOR_MatterModule* Module = Slot->GetModule<UTGOR_MatterModule>();
			Capacity += Module->GetStorageCapacity(Matter);
		}
	}
	return Capacity;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::PayStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition)
{
	TArray<UTGOR_ItemStorage*> Slots = GetItemSlots(FTGOR_ContainerIndex(0));

	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Payment = Composition;
	for (UTGOR_ItemStorage* Slot : Slots)
	{
		if (IsValid(Slot))
		{
			UTGOR_MatterModule* Module = Slot->GetModule<UTGOR_MatterModule>();
			Payment = Module->PayStorageMatter(Payment);
		}
	}
	OnMatterChanged.Broadcast();
	return Payment;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::InvestStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition)
{
	TArray<UTGOR_ItemStorage*> Slots = GetItemSlots(FTGOR_ContainerIndex(0));

	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Investment = Composition;
	for (UTGOR_ItemStorage* Slot : Slots)
	{
		if (IsValid(Slot))
		{
			UTGOR_MatterModule* Module = Slot->GetModule<UTGOR_MatterModule>();
			Investment = Module->InvestStorageMatter(Investment);
		}
	}
	OnMatterChanged.Broadcast();
	return Investment;
}
