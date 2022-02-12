// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "DimensionSystem/TGOR_DimensionInstance.h"
#include "TGOR_BodySpawnSetup.generated.h"

class UTGOR_Creature;

/* Automatically spawn a creature from current spectator */
UCLASS()
class PLAYERSYSTEM_API UTGOR_BodySpawnSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_BodySpawnSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Creature class to spawn with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Player", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Creature> DefaultBody;

	/* Body spawner to target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Player", Meta = (Keywords = "C++"))
		FTGOR_ConnectionSelection DefaultSpawner;
};