// The Gateway of Realities: Planes of Existence.

#include "TGOR_LoginSetup.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_WorldSettings.h"

#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "PlayerSystem/Data/TGOR_UserData.h"


#define LOCTEXT_NAMESPACE "TGOR_LoginSetup"

UTGOR_LoginSetup::UTGOR_LoginSetup()
	: Super(),
	AttemptedLogin(false)
{
	SetupName = LOCTEXT("LoginSetupName", "Login to server");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_LoginSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	// Only tries to login with online controllers but doesn't require it
	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(GetLocalPlayerController());
	if (!IsValid(OnlineController))
	{
		return Super::Attempt_Implementation(IsServer);
	}

	if (!AttemptedLogin)
	{
		OnlineController->RequestLogin();
		AttemptedLogin = true;
	}

	if(!OnlineController->IsLoggedIn())
	{
		SetLoadingStatus(LOCTEXT("LoginWaiting", "Waiting for server authentication..."));
		return false;
	}

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
