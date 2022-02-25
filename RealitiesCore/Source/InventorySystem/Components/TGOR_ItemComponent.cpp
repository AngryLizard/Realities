
#include "TGOR_ItemComponent.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "InventorySystem/Tasks/TGOR_ItemTask.h"
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
}

void UTGOR_ItemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ItemComponent, ItemSlots, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	TMap<UTGOR_Item*, TArray<UTGOR_ItemTask*>> Previous;

	// Remove slots but cache both instances and items in case the new loadout can use them
	for (int Slot = 0; Slot < ItemSlots.Num(); Slot++)
	{
		UTGOR_ItemTask* ItemSlot = ItemSlots[Slot];
		if (IsValid(ItemSlot))
		{
			Previous.FindOrAdd(ItemSlot->GetItem()).Add(ItemSlot);
		}
	}

	ItemSlots.Empty();

	// Get installed item modes
	TArray<UTGOR_Item*> Items = Dependencies.Spawner->GetMListFromType<UTGOR_Item>(SpawnItems);

	// Get all candidates that are part of the movement queue
	for (UTGOR_Item* Item : Items)
	{
		// Get slot from cache or create one
		UTGOR_ItemTask* ItemSlot = nullptr;

		TArray<UTGOR_ItemTask*>* Ptr = Previous.Find(Item);
		if (Ptr && Ptr->Num() > 0)
		{
			ItemSlot = Ptr->Pop();
		}
		else
		{
			// No cache was found, create a new one
			ItemSlot = Item->CreateItemTask(this, ItemSlots.Num());
		}

		// Initialise and add to slots
		if (IsValid(ItemSlot))
		{
			ItemSlots.Add(ItemSlot);
		}
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_ItemTask* ItemSlot : Pair.Value)
		{
			ItemSlot->MarkPendingKill();
		}
	}
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_ItemComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = ItemSlots.Num();
	for (UTGOR_ItemTask* ItemSlot : ItemSlots)
	{
		Modules.Emplace(ItemSlot->Identifier.Slot, ItemSlot->Identifier.Content);
	}
	return Modules;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<UTGOR_ItemTask*> UTGOR_ItemComponent::GetItemListOfType(TSubclassOf<UTGOR_ItemTask> Type) const
{
	TArray<UTGOR_ItemTask*> Items;
	if (*Type)
	{
		for (UTGOR_ItemTask* ItemSlot : ItemSlots)
		{
			if (ItemSlot->IsA(Type))
			{
				Items.Emplace(ItemSlot);
			}
		}
	}
	return Items;
}
