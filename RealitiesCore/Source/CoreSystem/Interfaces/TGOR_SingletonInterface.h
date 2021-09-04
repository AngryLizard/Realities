// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_SingletonInterface.generated.h"

#define SINGLETON_ERR "Couldn't load singleton"
#define SINGLETON_CHK if (!IsValid(EnsureSingleton(this))){ERROR(SINGLETON_ERR, Error)}
#define SINGLETON_RETCHK(T) if (!IsValid(EnsureSingleton(this))){ERRET(SINGLETON_ERR, Error, T)}
#define SINGLETON_CONSTCHK if (!IsValid(Singleton)){ERROR(SINGLETON_ERR, Error)}
#define SINGLETON_CONSTRETCHK(T) if (!IsValid(Singleton)){ERRET(SINGLETON_ERR, Error, T)}

class UTGOR_Singleton;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UTGOR_SingletonInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CORESYSTEM_API ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	ITGOR_SingletonInterface();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Ensures singleton */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		virtual UTGOR_Singleton* EnsureSingleton(const UObject* WorldContextObject) const;

	/** Overrides singleton with a custom one (mostly used for virtualisation). Will *not* update this object to reflect the change. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		virtual void OverrideSingleton(UTGOR_Singleton* NewSingleton);


protected:
	/** Singleton link */
	mutable TWeakObjectPtr<UTGOR_Singleton> Singleton;
};
