// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_Machine.generated.h"

class ATGOR_MachineActor;

UCLASS(Blueprintable)
class INVENTORYSYSTEM_API UTGOR_Machine : public UTGOR_Spawner
{
	GENERATED_BODY()

public:
	UTGOR_Machine();

	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Machine")
		TSubclassOf<AActor> Machine;

private:
};

