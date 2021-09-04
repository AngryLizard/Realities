// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionController.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_WorldComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

#include "GameFramework/GameState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

ATGOR_DimensionController::ATGOR_DimensionController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	WorldComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_WorldComponent>(this, FName(TEXT("WorldComponent")));
	WorldComponent->SetNetAddressable();
	WorldComponent->SetIsReplicated(true);
}

void ATGOR_DimensionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// This tick can happen very early and we don't want Singletoncheck to produce warnings
	UWorld* World = GetWorld();
	if (!IsValid(World->GetGameInstance()) ||
		!IsValid(World->GetGameState())) return;

	if (HasAuthority())
	{
		// Update requests for all players only on server
		WorldComponent->UpdateDimensionRequestsFromPawn(GetPawn());

		// Handle outstanding possessions
		if (HasPossessionQueue())
		{
			ProgressPossessionQueue();
		}
	}
	else if (IsLocalController())
	{
		// Server requests are updated by the gamestate, only do this on client
		if (!WorldComponent->IsDimensionListUpToDate())
		{
			WorldComponent->UpdateDimensionList();
		}
	}

	if (!WorldComponent->IsDimensionOwnerUpToDate())
	{
		WorldComponent->UpdateDimensionOwner();
	}
}

bool ATGOR_DimensionController::IsOnline() const
{
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		return WorldData->IsOnline(this);
	}
	return false;
}

void ATGOR_DimensionController::OnWorldUpdate()
{
}

void ATGOR_DimensionController::SetPossessionQueue(const FTGOR_DimensionIdentifier& Identifier)
{
	PossessionQueue = Identifier;

	// Immediately progress in case it's already valid
	ProgressPossessionQueue();
}


void ATGOR_DimensionController::ClearPossessionQueue()
{
	PossessionQueue = FTGOR_DimensionIdentifier();
}

bool ATGOR_DimensionController::HasPossessionQueue() const
{
	return PossessionQueue.DimensionIdentifier != "";
}

bool ATGOR_DimensionController::ProgressPossessionQueue()
{
	SINGLETON_RETCHK(false);

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		// Ignore invalid identifiers
		if (!WorldData->HasIdentifier(PossessionQueue.DimensionIdentifier))
		{
			ClearPossessionQueue();
			OnPossessionResponse(ETGOR_PossessionResponse::Invalid);
		}

		if (WorldData->HasFinishedLoading(PossessionQueue.DimensionIdentifier))
		{
			ETGOR_FetchEnumeration State;
			UTGOR_IdentityComponent* Identity = WorldData->GetIdentity(PossessionQueue, State);

			// Clear queue whether or not this step fails
			ClearPossessionQueue();
			if (State == ETGOR_FetchEnumeration::Found)
			{
				// Execute possesstion if pawn exists and isn't already possessed
				APawn* OtherPawn = Cast<APawn>(Identity->GetOwner());
				if (IsValid(OtherPawn))
				{
					if (OtherPawn->GetController() == nullptr)
					{
						if (ApplyPossession(OtherPawn))
						{
							OnPossessionResponse(ETGOR_PossessionResponse::Success);
							return true;
						}
					}
					else
					{
						OnPossessionResponse(ETGOR_PossessionResponse::Collision);
						return false;
					}
				}

			}
			OnPossessionResponse(ETGOR_PossessionResponse::Invalid);
			return false;
		}
		else if (WorldData->HasFinishedUnLoading(PossessionQueue.DimensionIdentifier) && !WorldComponent->HasDimensionRequest(PossessionQueue.DimensionIdentifier))
		{
			// Only try loading unloaded dimensions if they are properly unloaded and not already queued
			WorldComponent->AddExistingDimensionRequest(PossessionQueue.DimensionIdentifier);
		}

		OnPossessionResponse(ETGOR_PossessionResponse::Waiting);
	}
	return false;
}

bool ATGOR_DimensionController::ApplyPossession(APawn* OtherPawn)
{
	Possess(OtherPawn);
	return false;
}