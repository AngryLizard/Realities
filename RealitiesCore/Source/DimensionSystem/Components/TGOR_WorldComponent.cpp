// The Gateway of Realities: Planes of Existence.


#include "TGOR_WorldComponent.h"
#include "GameFramework/PlayerController.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionGameState.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_DimensionLoaderComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTGOR_WorldComponent::UTGOR_WorldComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);
}


void UTGOR_WorldComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to simulated
	DOREPLIFETIME_CONDITION(UTGOR_WorldComponent, DimensionRequestList, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UTGOR_WorldComponent, HasPendingDeletion, COND_OwnerOnly);
}


void UTGOR_WorldComponent::RepNotifyDimension(const FTGOR_DimensionRequestList& Old)
{
	UpdateDimensionList();
	UpdateDimensionOwner();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_WorldComponent::AddExistingDimensionRequest(const FName& Identifier)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);
		if (IsValid(Dimension))
		{
			AddDimensionRequest(Identifier, Dimension);
		}
	}
}

void UTGOR_WorldComponent::AddDimensionRequest(const FName& Identifier, UTGOR_Dimension* Dimension)
{
	SINGLETON_CHK;

	FTGOR_DimensionRequestEntry Entry;
	Entry.Content = Dimension;
	Entry.Timestamp = Singleton->GetGameTimestamp();
	DimensionRequestList.Dimensions.Add(Identifier, Entry);
}

void UTGOR_WorldComponent::RemoveDimensionRequest(const FName& Identifier)
{
	SINGLETON_CHK;

	int32 Count = DimensionRequestList.Dimensions.Remove(Identifier);
	if (Count > 0)
	{
		PendingDeletions.Add(Identifier, Singleton->GetGameTimestamp());
		HasPendingDeletion = true;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_WorldComponent::HasDimensionRequest(const FName& Identifier) const
{
	return DimensionRequestList.Dimensions.Contains(Identifier);
}


void UTGOR_WorldComponent::UpdateDimensionRequestsFromPawn(APawn* ControlledPawn)
{
	SINGLETON_CHK;

	TSet<FName> RequestIdentifiers;
	if (IsValid(ControlledPawn))
	{
		UTGOR_IdentityComponent* Identity = ControlledPawn->FindComponentByClass<UTGOR_IdentityComponent>();
		if (IsValid(Identity))
		{
			// Add owning dimension
			ETGOR_FetchEnumeration State;
			DimensionRequestList.Owner = Identity->GetDimensionIdentifier(State);
		}

		// Gather owned loaders
		RequestIdentifiers.Append(GetDimensionRequestsForActor(ControlledPawn));

		// Gather from parent
		RequestIdentifiers.Append(GetDimensionRequestsForPilot(ControlledPawn->FindComponentByClass<UTGOR_PilotComponent>()));
	}
	
	if(RequestIdentifiers.Num() == 0)
	{
		// Clients that just joined ought to load persistent level to get a spectator pawn
		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (IsValid(WorldData))
		{
			DimensionRequestList.Owner = WorldData->GetPersistentIdentifier();
			RequestIdentifiers.Add(DimensionRequestList.Owner);
		}
	}

	// Remove enabled dimensions
	for (FName RequestIdentifier : RequestIdentifiers)
	{
		if (!HasDimensionRequest(RequestIdentifier))
		{
			AddExistingDimensionRequest(RequestIdentifier);
		}
	}

	// Remove disabled dimensions
	for (auto Its = DimensionRequestList.Dimensions.CreateIterator(); Its; ++Its)
	{
		if (!RequestIdentifiers.Contains(Its->Key))
		{
			PendingDeletions.Add(Its->Key, Singleton->GetGameTimestamp());
			Its.RemoveCurrent();
			HasPendingDeletion = true;
		}
	}
}

void UTGOR_WorldComponent::UpdateDimensionRequestsFromPlayers()
{
	SINGLETON_CHK;

	TSet<FName> RequestIdentifiers;

	// Gather all dimensions loaded by at least one player
	UWorld* World = GetWorld();
	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		UTGOR_WorldComponent* WorldComponent = (*It)->FindComponentByClass<UTGOR_WorldComponent>();
		if (IsValid(WorldComponent))
		{
			for (const auto& Pair : WorldComponent->DimensionRequestList.Dimensions)
			{
				RequestIdentifiers.Emplace(Pair.Key);
			}
		}
	}

	// Server always has persistent loaded for joining clients
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		RequestIdentifiers.Add(WorldData->GetPersistentIdentifier());
	}

	// Remove enabled dimensions
	for (FName RequestIdentifier : RequestIdentifiers)
	{
		if (!HasDimensionRequest(RequestIdentifier))
		{
			AddExistingDimensionRequest(RequestIdentifier);
		}
	}

	// Remove disabled dimensions
	for (auto Its = DimensionRequestList.Dimensions.CreateIterator(); Its; ++Its)
	{
		if (!RequestIdentifiers.Contains(Its->Key))
		{
			PendingDeletions.Add(Its->Key, Singleton->GetGameTimestamp());
			Its.RemoveCurrent();
			HasPendingDeletion = true;
		}
	}
}

TSet<FName> UTGOR_WorldComponent::GetDimensionRequestsForActor(AActor* Actor)
{
	TSet<FName> Identifiers;
	// Add loaded dimensions (e.g. pocket dimension)
	TArray<UTGOR_DimensionLoaderComponent*> LoaderComponents;
	Actor->GetComponents(LoaderComponents);
	for (UTGOR_DimensionLoaderComponent* LoaderComponent : LoaderComponents)
	{
		Identifiers.Add(LoaderComponent->GetLoadedIdentifier());
	}
	return Identifiers;
}

TSet<FName> UTGOR_WorldComponent::GetDimensionRequestsForPilot(UTGOR_PilotComponent* Pilot)
{
	TSet<FName> Identifiers;
	if (IsValid(Pilot))
	{
		// Add current dimension
		ETGOR_FetchEnumeration State;
		Identifiers.Add(Pilot->GetDimensionIdentifier(State));

		UTGOR_MobilityComponent* Parent = Pilot->GetParent();
		if (IsValid(Parent))
		{
			// Get parent actor requests
			GetDimensionRequestsForActor(Parent->GetOwner());

			// Append parent dimensions
			Identifiers.Append(GetDimensionRequestsForPilot(Cast<UTGOR_PilotComponent>(Parent)));
		}
	}
	return Identifiers;
}

/*
void UTGOR_WorldComponent::SetDimensionOwner(const FName& Identifier)
{
	DimensionRequestList.Owner = Identifier;
}
*/

void UTGOR_WorldComponent::CopyDimensionRequestListFromHost()
{
	UWorld* World = GetWorld();
	ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(World->GetGameState());
	if (IsValid(GameState))
	{
		UTGOR_WorldComponent* WorldComponent = GameState->GetWorldManager();
		DimensionRequestList.Dimensions = WorldComponent->DimensionRequestList.Dimensions;
	}
}

void UTGOR_WorldComponent::UpdateDimensionList()
{
	if (DimensionRequestList.Dimensions.Num() == 0)
	{
		return;
	}

	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UWorld* World = GetWorld();
		const bool IsServer = HasServerAuthority();
		bool IsRealityUnloading = false;

		// Unload any loaded dimensions that are not in the update list
		TArray<FName> Keys;
		WorldData->GetLoadedDimensions(Keys);
		for (const FName& Key : Keys)
		{
			// Ignore already unloaded and persistent levels
			if (WorldData->HasBeenSpawned(Key) && Key != WorldData->GetPersistentIdentifier())
			{
				// Check whether dimension is in our update list
				bool HasDimension = false;
				for (const auto& Pair : DimensionRequestList.Dimensions)
				{
					// Make sure loaded dimensions with the wrong key are unloaded
					if (Pair.Key == Key)
					{
						HasDimension = true;
					}
				}

				/*
				// DONE: Server needs to also check all clients before unloading a dimension.
				// > The gamestate now also contains a WorldComponent and keeps track of player dimensions.
				// The way it is done here is very inefficient.
				if (IsServer)
				{
					for (auto It = World->GetPlayerControllerIterator(); It; ++It)
					{
						UTGOR_WorldComponent* WorldComponent = (*It)->FindComponentByClass<UTGOR_WorldComponent>();
						if (IsValid(WorldComponent))
						{
							for (const auto& Pair : WorldComponent->DimensionRequestList.Dimensions)
							{
								// Make sure loaded dimensions with the wrong key are unloaded
								if (Pair.Key == Key)
								{
									HasDimension = true;
								}
							}
						}
					}
				}
				*/

				if (!HasDimension)
				{
					if (WorldData->ProgressUnloadDimension(Key, IsServer))
					{
						// Let server know we have unloaded so host can continue unloading
						NotifyDimensionUnload(Key);
					}
					else if (WorldData->IsReality(Key))
					{
						IsRealityUnloading = true;
					}
				}
			}
		}

		// Load new dimensions
		for (auto& Pair : DimensionRequestList.Dimensions)
		{
			// Don't load a new reality while another reality is currently unloading
			if (!Pair.Value.Content->Reality || !IsRealityUnloading)
			{
				// Make sure dimension is dialed, also returns false if another dimension is currently unloading on the given identifier
				if (WorldData->SetIdentifier(Pair.Value.Content, Pair.Key))
				{
					// Progress if necessary
					if (WorldData->ProgressLoadDimension(Pair.Key, IsServer))
					{
						// Set location and content in case the values get changed by progressing and another controller wants to copy
						Pair.Value.Content = WorldData->GetDimensionContent(Pair.Key);
						Pair.Value.Timestamp = Singleton->GetGameTimestamp();
					}
				}
			}
		}
	}
}

bool UTGOR_WorldComponent::IsDimensionListUpToDate() const
{
	SINGLETON_RETCHK(false);
	if (DimensionRequestList.Dimensions.Num() == 0) return false;

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		if (HasPendingDeletion && HasServerAuthority()) return false;

		// TODO: Ignores dimensions about to be deleted
		for (const auto& Pair : DimensionRequestList.Dimensions)
		{
			if (!WorldData->HasFinishedLoading(Pair.Key))
			{
				return false;
			}
		}
	}
	return true;
}

void UTGOR_WorldComponent::UpdateDimensionOwner()
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		WorldData->SwitchActiveDimension(DimensionRequestList.Owner);
	}
}

bool UTGOR_WorldComponent::IsDimensionOwnerUpToDate() const
{
	SINGLETON_RETCHK(false);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		return (WorldData->GetActiveIdentifier() == DimensionRequestList.Owner);
	}
	return true;
}


void UTGOR_WorldComponent::NotifyDimensionUnload_Implementation(FName Identifier)
{
	PendingDeletions.Remove(Identifier);
	if (PendingDeletions.Num() == 0)
	{
		HasPendingDeletion = false;
	}
}

bool UTGOR_WorldComponent::NotifyDimensionUnload_Validate(FName Identifier)
{
	return true;
}
