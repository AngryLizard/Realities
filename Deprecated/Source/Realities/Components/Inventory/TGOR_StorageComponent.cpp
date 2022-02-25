// The Gateway of Realities: Planes of Existence.

#include "TGOR_StorageComponent.h"

#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Mod/Processes/TGOR_Science.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
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

	// Get sockets
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (TSubclassOf<UTGOR_NamedSocket> SocketClass : InputSockets)
	{
		Sockets.Append(ContentManager->GetTListFromType(SocketClass));
	}
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
	for (const auto& Pair : Attachments.Parts)
	{
		if (Pair.Value.Component.IsValid())
		{
			// Get item in socket instance
			UTGOR_ItemComponent* Component = Pair.Value.Component->GetOwnerComponent<UTGOR_ItemComponent>();
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
	TArray<UTGOR_NamedSocket*> List = GetSListOfType<UTGOR_NamedSocket>();
	for (UTGOR_NamedSocket* Socket : List)
	{
		if (CanFitToSocket(Shape, Socket))
		{
			return Socket;
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

