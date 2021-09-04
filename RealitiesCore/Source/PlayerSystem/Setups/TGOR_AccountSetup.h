// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_AccountSetup.generated.h"


UCLASS()
class PLAYERSYSTEM_API UTGOR_AccountSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_AccountSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;
};