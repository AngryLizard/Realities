// The Gateway of Realities: Planes of Existence.


#include "TGOR_WorldSettings.h"
#include "Engine/World.h"

#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Base/TGOR_GameState.h"

ATGOR_WorldSettings::ATGOR_WorldSettings()
:	Super(),
	ProfileOverride(""),
	SaveDirectory("SaveData"),
	TestEntry(true),
	TestCreature(UTGOR_Creature::StaticClass()),
	RealityMaxHeight(HALF_WORLD_MAX1 / 2),
	MiniDimensionsMaxHeight(HALF_WORLD_MAX1 / 2)
{
}

void ATGOR_WorldSettings::NotifyBeginPlay()
{
	// Make sure to progress gamestate before any begin play is issued
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		ATGOR_GameState* GameState = Cast<ATGOR_GameState>(World->GetGameState());
		if (IsValid(GameState))
		{
			ETGOR_BoolEnumeration State;
			GameState->Progress(State);
		}
	}
}