// The Gateway of Realities: Planes of Existence.

#include "TGOR_PossessionSetup.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"

#include "CoreSystem/TGOR_Singleton.h"

#include "CoreSystem/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "TGOR_PossessionSetup"


UTGOR_PossessionSetup::UTGOR_PossessionSetup()
	: Super()
{
	SetupName = LOCTEXT("PossessionSetupName", "Wait for spectator possession");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_PossessionSetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	// Wait until world is valid
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
		return false;
	}

	// If there is no local player we're running dedicated/server only
	APlayerController* LocalController = GetLocalPlayerController();
	if (!IsValid(LocalController))
	{
		return Super::Attempt_Implementation(IsServer);
	}

	// Wait until pawn has been possessed
	if (!IsValid(LocalController->GetPawn()))
	{
		SetLoadingStatus(LOCTEXT("PawnWaiting", "Waiting for player spawn..."));
		return false;
	}

	// Wait until HUD has loaded in
	// TODO: Check whether we can be absolutely sure a HUD exists after pawn has been possessed
	if (!IsValid(LocalController->GetHUD()))
	{
		SetLoadingStatus(LOCTEXT("HUDWaiting", "Waiting for HUD..."));
		return false;
	}

	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
