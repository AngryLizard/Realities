// The Gateway of Realities: Planes of Existence.

#include "TGOR_AwaitRequestsSetup.h"
#include "CoreSystem/TGOR_Singleton.h"


#include "DimensionSystem/Components/TGOR_WorldComponent.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionGameState.h"

#define LOCTEXT_NAMESPACE "TGOR_AwaitRequestsSetup"

UTGOR_AwaitRequestsSetup::UTGOR_AwaitRequestsSetup()
	: Super()
{
	SetupName = LOCTEXT("AwaitRequestsSetupName", "Wait for dimensions to load");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_AwaitRequestsSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
		return false;
	}
	
	UTGOR_WorldComponent* WorldComponent = nullptr;
	if (IsServer)
	{
		// On server the gamestate determines player loading, no need to check local controller
		ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(World->GetGameState());
		if (!IsValid(GameState))
		{
			SetLoadingStatus(LOCTEXT("GameplayWaiting", "Loading Game..."));
			return false;
		}

		WorldComponent = GameState->GetWorldManager();
	}
	else
	{
		APlayerController* LocalController = GetLocalPlayerController();
		if (!IsValid(LocalController))
		{
			SetLoadingStatus(LOCTEXT("PlayerWaiting", "Waiting for local player..."));
			return false;
		}

		WorldComponent = LocalController->FindComponentByClass<UTGOR_WorldComponent>();
	}

	if (!IsValid(WorldComponent))
	{
		SetLoadingStatus(LOCTEXT("ManagerWaiting", "Waiting for world manager..."));
		return false;
	}

	// Wait until all dimensions are loaded and ready
	WorldComponent->EnsureSingleton(this);
	if (!WorldComponent->IsDimensionListUpToDate())
	{
		WorldComponent->UpdateDimensionList();

		SetLoadingStatus(LOCTEXT("UpdateList", "Updating dimensions..."));
		return false;
	}

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
