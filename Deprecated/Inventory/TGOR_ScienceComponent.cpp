// The Gateway of Realities: Planes of Existence.

#include "TGOR_ScienceComponent.h"

#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Mod/Sciences/TGOR_Science.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Net/UnrealNetwork.h"


UTGOR_ScienceComponent::UTGOR_ScienceComponent()
	: Super(),
	ScienceRate(1.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_ScienceComponent::BeginPlay()
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

void UTGOR_ScienceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check whether processing is going on and done
	SINGLETON_CHK;
	if (IsValid(Recipe) && !IsReady && GetProcessingTime() <= 0.0f)
	{
		// Check again whether requirements are present
		if (IsCompleted(Recipe))
		{
			IsReady = true;

			OnStateChanged.Broadcast();
		}
	}
}

void UTGOR_ScienceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ScienceComponent::GetProcessingTime() const
{
	if (IsValid(Singleton))
	{
		FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
		return Deadline - Timestamp;
	}
	return 0.0f;
}

float UTGOR_ScienceComponent::GetProcessingProgress() const
{
	if (IsValid(Recipe))
	{
		float Time = GetResearchTime(Recipe);
		return 1.0f - (GetProcessingTime() / FMath::Max(Time, 0.01f));
	}
	return 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ScienceComponent::GetResearchTime(UTGOR_Science* Science) const
{
	float Mass = 0.0f;

	// Compute total attached mass
	TArray<UTGOR_ItemStorage*> Storages = GetAttachedStorages();
	for (UTGOR_ItemStorage* Storage : Storages)
	{
		Mass += Storage->GetMass();
	}

	// Compute time when the experiment is done
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	return Mass * Science->Rate * ScienceRate;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ScienceComponent::ScheduleScience(UTGOR_Science* Science)
{
	SINGLETON_CHK;
	if (IsCompleted(Science))
	{
		// Compute time when the experiment is done
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
		Deadline = Timestamp + GetResearchTime(Science);
		Recipe = Science;
		IsReady = false;

		OnStateChanged.Broadcast();
	}
}

bool UTGOR_ScienceComponent::IsCompleted(UTGOR_Science* Science) const
{
	TArray<UTGOR_Item*> Items = GetAttachedItems();

	// Trivially check whether we have enough items
	if (Items.Num() < Science->ItemInputs.Num()) return false;

	for (TSubclassOf<UTGOR_Item> ItemInput : Science->ItemInputs)
	{
		for (auto It = Items.CreateIterator(); It; ++It)
		{
			if ((*It)->IsA(ItemInput))
			{
				It.RemoveCurrent();
			}
		}
	}
		
	return Items.Num() == 0;
}

TArray<UTGOR_Item*> UTGOR_ScienceComponent::GetAttachedItems() const
{
	TArray<UTGOR_Item*> Output;

	TArray<UTGOR_ItemStorage*> Storages = GetAttachedStorages();
	for (UTGOR_ItemStorage* Storage : Storages)
	{
		UTGOR_Item* Item = Storage->GetItem();
		Output.Emplace(Item);
	}

	return Output;
}

TArray<UTGOR_ItemStorage*> UTGOR_ScienceComponent::GetAttachedStorages() const
{
	TArray<UTGOR_ItemStorage*> Output;
	for (const auto& Pair : Attachments.Parts)
	{
		// Get item in socket instance
		UTGOR_ItemComponent* Component = Pair.Value.Component->GetOwnerComponent<UTGOR_ItemComponent>();
		if (IsValid(Component))
		{
			UTGOR_ItemStorage* Storage = Component->PeekStorage();
			Output.Emplace(Storage);
		}
	}
	return Output;
}
