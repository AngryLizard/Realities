// The Gateway of Realities: Planes of Existence.

#include "TGOR_BodySpawnSetup.h"
#include "GameFramework/PlayerController.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "PlayerSystem/Components/TGOR_BodySpawnComponent.h"
#include "PlayerSystem/Content/TGOR_DefaultPawnSpawner.h"
#include "PlayerSystem/Actors/TGOR_Spectator.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

#define LOCTEXT_NAMESPACE "TGOR_BodySpawnSetup"

UTGOR_BodySpawnSetup::UTGOR_BodySpawnSetup()
	: Super()
{
	SetupName = LOCTEXT("SpawnSetupName", "Spawn player");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_BodySpawnSetup::Attempt_Implementation(bool IsServer)
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

		ETGOR_FetchEnumeration Fetch;
		FName Identifier = WorldData->FindIdentifier(DefaultSpawner.Dimension, DefaultSpawner.Suffix, Fetch);
		if (Fetch == ETGOR_FetchEnumeration::Empty)
		{
			return SetFinishedStatus(LOCTEXT("NoDimension", "Spawn Dimension doesn't exist, skipping..."));
		}

		UTGOR_DimensionData* DimensionData = WorldData->GetDimension(Identifier, Fetch);
		if (!IsValid(DimensionData))
		{
			return SetFinishedStatus(LOCTEXT("NoDimension", "Spawn Dimension isn't loaded, skipping..."));
		}

		UTGOR_BodySpawnComponent* SpawnerConnection = DimensionData->GetCOfType<UTGOR_BodySpawnComponent>(DefaultSpawner.Connection);
		if (!IsValid(SpawnerConnection))
		{
			SetLoadingStatus(LOCTEXT("ConnectionWaiting", "Getting connection..."));
			return false;
		}

		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		UTGOR_Creature* Creature = ContentManager->GetTFromType<UTGOR_Creature>(DefaultBody);
		if (IsValid(Creature))
		{
			FTGOR_CreateBodySetup Setup;
			Setup.Creature = Creature;
			SpawnerConnection->RequestNewBody(LocalController, Setup, true);
		}
		else
		{
			return SetFinishedStatus(LOCTEXT("NoCreature", "No creature defined, skipping..."));
		}
	}
	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
