// The Gateway of Realities: Planes of Existence.


#include "TGOR_WorldSettings.h"
#include "Engine/World.h"

#include "TGOR_GameMode.h"
#include "TGOR_GameState.h"


ATGOR_WorldSettings::ATGOR_WorldSettings()
:	Super(),
	ProfileOverride(""),
	SaveDirectory("SaveData")
{
}

void ATGOR_WorldSettings::NotifyBeginPlay()
{
	// Make sure to progress gamestate before any begin play is issued
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		// When rendering sequences we have a gamemode that isn't our TGOR_Gamemode, start BeginPlay so we start rendering properly
		AGameModeBase* GameMode = World->GetAuthGameMode();
		if (IsValid(GameMode) && !GameMode->IsA<ATGOR_GameMode>())
		{
			Super::NotifyBeginPlay();
			return;
		}

		ATGOR_GameState* GameState = Cast<ATGOR_GameState>(World->GetGameState());
		if (IsValid(GameState))
		{
			GameState->Setup();
			GameState->Progress();
		}
	}

	// We don't call Super::NotifyBeginPlay here, begin play ought to be called as part for the setup process
}