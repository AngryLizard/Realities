// The Gateway of Realities: Planes of Existence.

#include "TGOR_PersistentSetup.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "DimensionSystem/Components/TGOR_WorldComponent.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionWorldSettings.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionGameState.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"

#define LOCTEXT_NAMESPACE "TGOR_PersistentSetup"

UTGOR_PersistentSetup::UTGOR_PersistentSetup()
	: Super()
{
	SetupName = LOCTEXT("PersistentSetupName", "Assemble World");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_PersistentSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
		return false;
	}

	ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(World->GetGameState());
	ATGOR_DimensionWorldSettings* Settings = Cast<ATGOR_DimensionWorldSettings>(World->GetWorldSettings());
	if (!IsValid(Settings) || !IsValid(GameState))
	{
		SetLoadingStatus(LOCTEXT("GameplayWaiting", "Loading gameplay systems..."));
		return false;
	}

	// Set up world
	UTGOR_WorldData* WorldData = AddData<UTGOR_WorldData>();
	if (!IsValid(WorldData))
	{
		SetLoadingStatus(LOCTEXT("DataWaiting", "Loading world data..."));
		return false;
	}

	WorldData->PersistentWorld = World;
	WorldData->RealityHeight = Settings->RealityMaxHeight;
	WorldData->MiniDimensionHeight = Settings->MiniDimensionsMaxHeight;

	// Load persistent
	if (*Settings->Dimension)
	{
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		UTGOR_Dimension* Dimension = ContentManager->GetTFromType<UTGOR_Dimension>(Settings->Dimension);
		if (!IsValid(Dimension))
		{
			SetLoadingStatus(LOCTEXT("PersistenWaiting", "Waitng for entry dimension..."));
			return false;
		}

		// Always assemble persistent level since it is already loaded
		WorldData->AssemblePersistent(Dimension, IsServer);

		// Initialise server dimensions
		if (IsServer)
		{
			// Local player is not ticking yet so we need to update dimensions for them specifically
			APlayerController* LocalController = GetLocalPlayerController();
			if (IsValid(LocalController))
			{
				// Update player dimension to make sure we're loading everything the player is supposed to see after load
				UTGOR_WorldComponent* WorldComponent = LocalController->FindComponentByClass<UTGOR_WorldComponent>();
				if (!IsValid(WorldComponent))
				{
					SetLoadingStatus(LOCTEXT("ClientManagerWaiting", "Used controller doesn't own a WorldComponent..."));
					return false;
				}

				WorldComponent->UpdateDimensionRequestsFromPawn(LocalController->GetPawn());
			}


			// Load what players requested (usually only persistent)
			UTGOR_WorldComponent* ServerWorldComponent = GameState->GetWorldManager();
			if (!IsValid(ServerWorldComponent))
			{
				SetLoadingStatus(LOCTEXT("ServerManagerWaiting", "Waiting for world manager..."));
				return false;
			}

			ServerWorldComponent->UpdateDimensionRequestsFromPlayers();
		}
	}

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
