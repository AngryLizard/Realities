// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_ModSetup.h"
#include "GameFramework/PlayerController.h"


#define LOCTEXT_NAMESPACE "TGOR_ModSetup"

UTGOR_ModSetup::UTGOR_ModSetup()
	: Super(),
	SetupOwner(ETGOR_SetupOwnerEnumeration::All)
{
	SetupName = LOCTEXT("DefaultSetupName", "Mod Setup");

	State.Status = LOCTEXT("DefaultSetupWaiting", "Pending...");
	State.State = ETGOR_SetupStateEnumeration::Pending;
}

void UTGOR_ModSetup::SetLoadingStatus(FText Text)
{
	State.Status = Text;
	State.State = ETGOR_SetupStateEnumeration::Loading;
}

bool UTGOR_ModSetup::Attempt_Implementation(bool IsServer)
{
	State.Status = LOCTEXT("DefaultSetupFinished", "Done!");
	State.State = ETGOR_SetupStateEnumeration::Finished;
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

APlayerController* UTGOR_ModSetup::GetLocalPlayerController() const
{
	UWorld* World = GetWorld();
	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (It->Get()->IsLocalController())
		{
			return It->Get();
		}
	}
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
