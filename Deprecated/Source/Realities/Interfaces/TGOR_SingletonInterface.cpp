// The Gateway of Realities: Planes of Existence.


#include "TGOR_SingletonInterface.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"

// Add default functionality here for any ITGOR_SingletonInterface functions that are not pure virtual.

ITGOR_SingletonInterface::ITGOR_SingletonInterface()
{
	Singleton = nullptr;
}

UTGOR_Singleton* ITGOR_SingletonInterface::EnsureSingleton(UObject* WorldContextObject)
{
	// Only load singleton if not already cached
	if (IsValid(Singleton))
	{
		return(Singleton);
	}

	// Get singleton from GameInstance
	Singleton = UTGOR_GameInstance::EnsureSingletonFromWorld(WorldContextObject->GetWorld());
	if (!IsValid(Singleton))
	{
		ERRET("Couldn't load singleton", Fatal, nullptr)
	}
	return(Singleton);
}