// The Gateway of Realities: Planes of Existence.


#include "TGOR_GameMode.h"

#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Actors/Pawns/TGOR_Spectator.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

ATGOR_GameMode::ATGOR_GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bStartPlayersAsSpectators = true;
}

APawn* ATGOR_GameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}
