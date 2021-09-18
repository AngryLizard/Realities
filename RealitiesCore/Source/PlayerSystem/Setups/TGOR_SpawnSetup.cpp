// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpawnSetup.h"
#include "GameFramework/PlayerController.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CreatureSystem/Content/TGOR_DefaultCreature.h"
#include "PlayerSystem/Actors/TGOR_Spectator.h"


#define LOCTEXT_NAMESPACE "TGOR_SpawnSetup"

UTGOR_SpawnSetup::UTGOR_SpawnSetup()
	: Super()
{
	SetupName = LOCTEXT("SpawnSetupName", "Spawn player");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;

	DefaultBody = UTGOR_DefaultCreature::StaticClass();
	OnlySpawnServerBody = false;
}

bool UTGOR_SpawnSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	// Only spawn player if we're not dedicated server
	APlayerController* LocalController = GetLocalPlayerController();
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

	if (IsServer || !OnlySpawnServerBody)
	{
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
				Spectator->RequestNewBody(Setup);
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
