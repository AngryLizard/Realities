// The Gateway of Realities: Planes of Existence.
#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Base/Controller/TGOR_PlayerController.h"

#include "GameFramework/GameMode.h"
#include "TGOR_GameMode.generated.h"

class UTGOR_SpectatorSpawner;

/**
 * 
 */
UCLASS()
class REALITIES_API ATGOR_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATGOR_GameMode(const FObjectInitializer& ObjectInitializer);

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

};