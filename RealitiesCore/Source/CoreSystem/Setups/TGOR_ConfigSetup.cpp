// The Gateway of Realities: Planes of Existence.

#include "TGOR_ConfigSetup.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Data/TGOR_ConfigData.h"

#define LOCTEXT_NAMESPACE "TGOR_ConfigSetup"


UTGOR_ConfigSetup::UTGOR_ConfigSetup()
	: Super()
{
	SetupName = LOCTEXT("ConfigSetupName", "Load local player information");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_ConfigSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
		return false;
	}

	UTGOR_ConfigData* ConfigData = AddData<UTGOR_ConfigData>();
	if (!IsValid(ConfigData))
	{
		SetLoadingStatus(LOCTEXT("CreateData", "Creating config data..."));
		return false;
	}

	// Load local files for client and server
	ConfigData->SetFilename("Config");
	ConfigData->LoadFromFile();

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
