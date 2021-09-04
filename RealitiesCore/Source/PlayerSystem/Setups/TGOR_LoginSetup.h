// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Setups/TGOR_Setup.h"
#include "TGOR_LoginSetup.generated.h"


UCLASS()
class PLAYERSYSTEM_API UTGOR_LoginSetup : public UTGOR_Setup
{
	GENERATED_BODY()

public:
	UTGOR_LoginSetup();
	virtual bool Attempt_Implementation(bool IsServer) override;

private:

	/** Whether we've tried to login already so we don't send a request every tick */
	UPROPERTY()
		bool AttemptedLogin;
};