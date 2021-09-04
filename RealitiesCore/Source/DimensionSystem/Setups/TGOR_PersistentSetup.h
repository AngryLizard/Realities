// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_PersistentSetup.generated.h"


UCLASS()
class DIMENSIONSYSTEM_API UTGOR_PersistentSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_PersistentSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;
};