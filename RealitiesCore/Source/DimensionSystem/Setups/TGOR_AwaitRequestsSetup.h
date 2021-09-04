// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_AwaitRequestsSetup.generated.h"


UCLASS()
class DIMENSIONSYSTEM_API UTGOR_AwaitRequestsSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_AwaitRequestsSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;
};