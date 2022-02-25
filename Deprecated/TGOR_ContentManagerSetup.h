// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Realities/CoreSystem/Singleton/Setups/TGOR_Setup.h"
#include "TGOR_ContentManagerSetup.generated.h"


UCLASS()
class REALITIES_API UTGOR_ContentManagerSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_ContentManagerSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;
};
