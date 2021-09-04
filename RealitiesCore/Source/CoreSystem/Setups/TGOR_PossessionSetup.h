// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_PossessionSetup.generated.h"


UCLASS()
class CORESYSTEM_API UTGOR_PossessionSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_PossessionSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;
};