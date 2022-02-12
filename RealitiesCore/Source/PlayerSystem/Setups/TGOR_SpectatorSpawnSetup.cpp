// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpectatorSpawnSetup.h"
#include "GameFramework/PlayerController.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "PlayerSystem/Content/TGOR_DefaultPawnSpawner.h"
#include "PlayerSystem/Actors/TGOR_Spectator.h"


#define LOCTEXT_NAMESPACE "TGOR_SpectatorSpawnSetup"

UTGOR_SpectatorSpawnSetup::UTGOR_SpectatorSpawnSetup()
	: Super()
{
	SetupName = LOCTEXT("SpawnSetupName", "Spawn player");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;

	DefaultBody = UTGOR_Creature::StaticClass();
}

bool UTGOR_SpectatorSpawnSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	if (IsServer)
	{
		// Only spawn player if we're not dedicated server
		ATGOR_OnlineController* LocalController = Cast<ATGOR_OnlineController>(GetLocalPlayerController());
		if (!IsValid(LocalController))
		{
			return Super::Attempt_Implementation(IsServer);
		}

		UWorld* World = GetWorld();
		if (!IsValid(World))
		{
			SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
			return false;
		}

		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (!IsValid(WorldData))
		{
			SetLoadingStatus(LOCTEXT("WorldDataWaiting", "Loading World data..."));
			return false;
		}

		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		UTGOR_Creature* Creature = ContentManager->GetTFromType<UTGOR_Creature>(DefaultBody);
		if (IsValid(Creature))
		{
			// If there was no save, spawn default pawn immediately
			APawn* Pawn = LocalController->GetPawn();
			ATGOR_Spectator* Spectator = Cast<ATGOR_Spectator>(Pawn);
			if (IsValid(Spectator))
			{
				FTGOR_CreateBodySetup Setup;
				Setup.Creature = Creature;
				Spectator->GetSpawn()->RequestNewBody(LocalController, Setup, true);
			}
			else
			{
				SetLoadingStatus(LOCTEXT("SpectatorWaiting", "Getting current spectator..."));
				return false;
			}
		}
		else
		{
			return SetFinishedStatus(LOCTEXT("NoCreature", "No creature defined, skipping..."));
		}
	}
	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
