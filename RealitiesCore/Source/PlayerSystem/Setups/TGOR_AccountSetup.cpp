// The Gateway of Realities: Planes of Existence.

#include "TGOR_AccountSetup.h"

#include "CoreSystem/TGOR_Singleton.h"

#include "PlayerSystem/Data/TGOR_UserData.h"


#define LOCTEXT_NAMESPACE "TGOR_AccountSetup"

UTGOR_AccountSetup::UTGOR_AccountSetup()
	: Super()
{
	SetupName = LOCTEXT("AccountSetupName", "Load user accounts");
	SetupOwner = ETGOR_SetupOwnerEnumeration::ServerOnly;
}

bool UTGOR_AccountSetup::Attempt_Implementation(bool IsServer)
{
	State.State = State.State = ETGOR_SetupStateEnumeration::Loading;
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
		return false;
	}

	UTGOR_UserData* UserData = AddData<UTGOR_UserData>();
	if (!IsValid(UserData))
	{
		SetLoadingStatus(LOCTEXT("DataWaiting", "Loading user accounts..."));
		return false;
	}

	UserData->SetFilename("Users");
	UserData->LoadFromFile();

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
