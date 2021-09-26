// The Gateway of Realities: Planes of Existence.

#pragma once
#include "CoreMinimal.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_DefaultPawnSpawner.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class PLAYERSYSTEM_API UTGOR_DefaultPawnSpawner : public UTGOR_Spawner
{
	GENERATED_BODY()

public:
	UTGOR_DefaultPawnSpawner();
	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

private:
};

