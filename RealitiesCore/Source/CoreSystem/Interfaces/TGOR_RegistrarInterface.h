// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_RegisterInterface.h"
#include "../TGOR_HandleInstance.h"

#include "UObject/Interface.h"
#include "TGOR_RegistrarInterface.generated.h"

class UTGOR_Content;


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_RegistrarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CORESYSTEM_API ITGOR_RegistrarInterface
{
	GENERATED_BODY()

public:

	/** Deregister given register */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		virtual void UnregisterHandle(const FTGOR_HandleInstance& Handle);

	/** Deregister given register */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		virtual void UnregisterHandleRegister(TScriptInterface<ITGOR_RegisterInterface> Register);

	/** Deregister given key */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		virtual void UnregisterHandleKey(UTGOR_Content* Key);

	/** Deregister all handles */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		virtual void UnregisterAll();


protected:

	/** Called *after* a register is removed from this registrar */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		virtual void OnUnregister(const FTGOR_HandleInstance& Handle);

private:
};
