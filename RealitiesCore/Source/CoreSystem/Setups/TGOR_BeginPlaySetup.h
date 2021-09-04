// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_BeginPlaySetup.generated.h"

UCLASS()
class CORESYSTEM_API UTGOR_BeginPlaySetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_BeginPlaySetup();
	virtual bool Attempt_Implementation(bool IsServer) override;
};
