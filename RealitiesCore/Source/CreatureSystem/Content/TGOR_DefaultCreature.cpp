// The Gateway of Realities: Planes of Existence.

#include "TGOR_DefaultCreature.h"
#include "GameFramework/GameModeBase.h"

UTGOR_DefaultCreature::UTGOR_DefaultCreature()
	: Super()
{
}

void UTGOR_DefaultCreature::BuildResource()
{
	Super::BuildResource();

	UWorld* World = GetWorld();
	AGameModeBase* GameMode = World->GetAuthGameMode();
	Pawn = GameMode->DefaultPawnClass;
}