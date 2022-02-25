// The Gateway of Realities: Planes of Existence.

#include "TGOR_ContentManagerSetup.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "Realities/CoreSystem/Gameplay/TGOR_GameState.h"
#include "Realities/CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "Realities/CoreSystem/Gameplay/TGOR_WorldSettings.h"
#include "Realities/CoreSystem/Singleton/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "TGOR_ContentManagerSetup"


UTGOR_ContentManagerSetup::UTGOR_ContentManagerSetup()
	: Super()
{
	SetupName = LOCTEXT("ContentManagerSetupName", "Load all mods");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_ContentManagerSetup::Attempt_Implementation(bool IsServer)
{
	State.State = State.State = ETGOR_SetupStateEnumeration::Loading;
	State.Status = LOCTEXT("SingletonWaiting", "Creating Singleton...");
	SINGLETON_RETCHK(false);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		State.Status = LOCTEXT("WorldWaiting", "Loading World...");
		return false;
	}

	// Wait until instance is valid
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	ATGOR_GameState* GameState = Cast<ATGOR_GameState>(World->GetGameState());
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
	if (!IsValid(ContentManager) || !IsValid(GameState) || !IsValid(Instance) || !IsValid(Settings))
	{
		State.Status = LOCTEXT("GameplayWaiting", "Loading Gameplay systems...");
		return false;
	}

	ETGOR_ModloadEnumeration ModLoad;
	if (IsServer)
	{
		// Load locally
		ModLoad = ContentManager->LoadLocalModSetup(Instance->UGCRegistry, Settings->CoreMod);
	}
	else
	{
		ModLoad = ContentManager->LoadModSetup(Instance->UGCRegistry);
	}

	if (ModLoad != ETGOR_ModloadEnumeration::Success)
	{
		State.Status = LOCTEXT("LoadWaiting", "Loading mods...");
		return false;
	}

	// Notify gamestate
	GameState->OnModsLoadComplete(ModLoad);

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
