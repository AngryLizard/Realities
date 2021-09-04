// The Gateway of Realities: Planes of Existence.

#include "TGOR_StorageComponent.h"

#include "SocketSystem/Content/TGOR_NamedSocket.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Storage/Modules/TGOR_MatterModule.h"
#include "SocketSystem/Tasks/TGOR_NamedSocketTask.h"
#include "InventorySystem/Content/TGOR_Matter.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"


////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_StorageComponent::UTGOR_StorageComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_StorageComponent::BeginPlay()
{
	Super::BeginPlay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////


TArray<UTGOR_Item*> UTGOR_StorageComponent::GetAttachedItems() const
{
	TArray<UTGOR_Item*> Output;

	TArray<UTGOR_ItemComponent*> Containers = GetAttachedContainers();
	for (UTGOR_ItemComponent* Container : Containers)
	{
		UTGOR_ItemStorage* Storage = Container->PeekStorage();
		UTGOR_Item* Item = Storage->GetItem();
		Output.Emplace(Item);
	}
	return Output;
}

TArray<UTGOR_ItemComponent*> UTGOR_StorageComponent::GetAttachedContainers() const
{
	TArray<UTGOR_ItemComponent*> Output;
	for (const auto& Pair : Attachments)
	{
		if (Pair.Value.IsValid())
		{
			// Get item in socket instance
			UTGOR_ItemComponent* Component = Pair.Value->Identifier.Component->GetOwnerComponent<UTGOR_ItemComponent>();
			if (IsValid(Component))
			{
				Output.Emplace(Component);
			}
		}
	}
	return Output;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_NamedSocket* UTGOR_StorageComponent::GetEmptySocketFor(UTGOR_Item* Item) const
{
	const FTGOR_MovementShape Shape = Item->GetShape();
	for (const auto& Pair : Attachments)
	{
		if (CanFitToSocket(Shape, Pair.Key))
		{
			return Pair.Key;
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_StorageComponent::GetMissing() const
{
	TArray<UTGOR_ItemComponent*> Containers = GetAttachedContainers();

	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Missing;
	for (UTGOR_ItemComponent* Container : Containers)
	{
		UTGOR_ItemStorage* Slot = Container->PeekStorage();
		Slot->CombineMissingOnto(Missing);
	}
	return Missing;
}

TMap<UTGOR_Matter*, int32> UTGOR_StorageComponent::GetComposition() const
{
	TArray<UTGOR_ItemComponent*> Containers = GetAttachedContainers();

	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Composition;
	for (UTGOR_ItemComponent* Container : Containers)
	{
		UTGOR_ItemStorage* Slot = Container->PeekStorage();
		Slot->CombineCompositionOnto(Composition);
	}
	return Composition;
}

TMap<UTGOR_Matter*, int32> UTGOR_StorageComponent::RemoveComposition(const TArray<UTGOR_ItemComponent*>& Containers, TMap<UTGOR_Matter*, int32> Composition)
{
	TMap<UTGOR_Matter*, int32> Output;
	for (const auto& Pair : Composition)
	{
		const int32 Quantity = RemoveMatter(Containers, Pair.Key, Pair.Value);
		Output.Emplace(Pair.Key, Quantity);
	}
	return Output;
}

int32 UTGOR_StorageComponent::RemoveMatter(const TArray<UTGOR_ItemComponent*>& Containers, UTGOR_Matter* Matter, int32 Quantity)
{
	int32 Count = 0;

	// Randomly permutate input containers
	TArray<UTGOR_ItemComponent*> Queue = Containers;
	const int32 Last = Queue.Num() - 1;
	for (int32 i = 0; i < Last; i++)
	{
		Queue.Swap(i, ContainerPermutator.RandRange(i, Last));
	}

	// Remove matter in order, remove items that are drained
	for (UTGOR_ItemComponent* Container : Queue)
	{
		UTGOR_ItemStorage* Slot = Container->PeekStorage();
		Count += Slot->RemoveMatter(Matter, Quantity - Count);

		if (!Slot->HasAnyMatter())
		{
			Slot = Container->PickItem();
			UTGOR_ContainerComponent* Component = GetOwnerComponent<UTGOR_ContainerComponent>();
			if (IsValid(Component))
			{
				Component->DestroyItemStorage(Slot);
			}
		}
	}
	return Count;
}

TMap<UTGOR_Matter*, int32> UTGOR_StorageComponent::AddComposition(const TArray<UTGOR_ItemComponent*>& Containers, TMap<UTGOR_Matter*, int32> Composition)
{
	TMap<UTGOR_Matter*, int32> Output;
	for (const auto& Pair : Composition)
	{
		const int32 Quantity = AddMatter(Containers, Pair.Key, Pair.Value);
		Output.Emplace(Pair.Key, Quantity);
	}
	return Output;
}

int32 UTGOR_StorageComponent::AddMatter(const TArray<UTGOR_ItemComponent*>& Containers, UTGOR_Matter* Matter, int32 Quantity)
{
	// Combine all item compositions
	for (UTGOR_ItemComponent* Container : Containers)
	{
		UTGOR_ItemStorage* Slot = Container->PeekStorage();
		Quantity = Slot->AddMatter(Matter, Quantity);
	}
	return Quantity;
}

