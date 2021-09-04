// The Gateway of Realities: Planes of Existence.

#include "TGOR_BeginPlaySetup.h"
#include "EngineUtils.h"

#include "CoreSystem/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "TGOR_BeginPlaySetup"

UTGOR_BeginPlaySetup::UTGOR_BeginPlaySetup()
	: Super()
{
	SetupName = LOCTEXT("BeginPlaySetupName", "Begin Play Setup");
	SetupOwner = ETGOR_SetupOwnerEnumeration::All;
}

bool UTGOR_BeginPlaySetup::Attempt_Implementation(bool IsServer)
{
	SetLoadingStatus(LOCTEXT("SingletonWaiting", "Creating Singleton..."));
	SINGLETON_RETCHK(false);

	// Call BeginPlay if necessary
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		SetLoadingStatus(LOCTEXT("WorldWaiting", "Loading World..."));
		return false;
	}

	if (!World->bBegunPlay)
	{
		for (FActorIterator It(World); It; ++It)
		{
			It->DispatchBeginPlay(true);
		}
		World->bBegunPlay = true;
	}
	World->SetShouldTick(true);
	return Super::Attempt_Implementation(IsServer);
}

#undef LOCTEXT_NAMESPACE
