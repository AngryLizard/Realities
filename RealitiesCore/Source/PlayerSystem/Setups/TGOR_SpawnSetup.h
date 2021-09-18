// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_SpawnSetup.generated.h"

class UTGOR_Creature;

/* Automatically spawn a creature from current spectator */
UCLASS()
class PLAYERSYSTEM_API UTGOR_SpawnSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_SpawnSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Dimension this world is associated with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Player", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Creature> DefaultBody;

	/* Spawn server body only (skip clients) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Player", Meta = (Keywords = "C++"))
		bool OnlySpawnServerBody;
};