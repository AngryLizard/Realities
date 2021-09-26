// The Gateway of Realities: Planes of Existence.

#include "TGOR_DefaultPawnSpawner.h"
#include "GameFramework/GameModeBase.h"

UTGOR_DefaultPawnSpawner::UTGOR_DefaultPawnSpawner()
	: Super()
{
	Abstract = false;
}

TSubclassOf<AActor> UTGOR_DefaultPawnSpawner::GetSpawnClass_Implementation() const
{
	UWorld* World = GetWorld();
	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (IsValid(GameMode))
	{
		return GameMode->DefaultPawnClass;
	}
	else
	{
		return APawn::StaticClass();
	}
}
