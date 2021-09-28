// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Gameplay/TGOR_GameMode.h"
#include "TGOR_DimensionGameMode.generated.h"

class UTGOR_Spawner;

/**
 * 
 */
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API ATGOR_DimensionGameMode : public ATGOR_GameMode
{
	GENERATED_BODY()
	
public:
	ATGOR_DimensionGameMode(const FObjectInitializer& ObjectInitializer);

	APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

};
