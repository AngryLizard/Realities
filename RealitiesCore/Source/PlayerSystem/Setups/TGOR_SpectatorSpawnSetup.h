// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_SpectatorSpawnSetup.generated.h"

class UTGOR_Creature;

/* Automatically spawn a creature from current spectator */
UCLASS()
class PLAYERSYSTEM_API UTGOR_SpectatorSpawnSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_SpectatorSpawnSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Creature class to spawn with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Player", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Creature> DefaultBody;
};