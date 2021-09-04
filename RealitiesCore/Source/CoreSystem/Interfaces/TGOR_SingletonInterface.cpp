// The Gateway of Realities: Planes of Existence.


#include "TGOR_SingletonInterface.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

// Add default functionality here for any ITGOR_SingletonInterface functions that are not pure virtual.

ITGOR_SingletonInterface::ITGOR_SingletonInterface()
{
	Singleton = nullptr;
}


UTGOR_Singleton* FindOuterSingleton(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	// Look for singleton in outer first
	UObject* Outer = WorldContextObject->GetOuter();
	if (IsValid(Outer))
	{
		ITGOR_SingletonInterface* SingletonInterface = Cast<ITGOR_SingletonInterface>(Outer);
		if (SingletonInterface)
		{
			return SingletonInterface->EnsureSingleton(Outer);
		}

		// Recursive call
		return FindOuterSingleton(Outer);
	}

	return nullptr;
}


UTGOR_Singleton* ITGOR_SingletonInterface::EnsureSingleton(const UObject* WorldContextObject) const
{
	if (!IsValid(WorldContextObject))
	{
		return Singleton.Get();
	}

	// Only load singleton if not already cached
	if (Singleton.IsValid())
	{
		return Singleton.Get();
	}

	// Traverse outerchain for singleton outer
	Singleton = FindOuterSingleton(WorldContextObject);
	if (Singleton.IsValid())
	{
		return Singleton.Get();
	}

	// Get singleton from GameInstance if nothing else worked
	Singleton = UTGOR_GameInstance::EnsureSingletonFromWorld(WorldContextObject->GetWorld());
	if (!Singleton.IsValid())
	{
		ERRET("Couldn't load singleton", Fatal, nullptr)
	}
	return Singleton.Get();
}


void ITGOR_SingletonInterface::OverrideSingleton(UTGOR_Singleton* NewSingleton)
{
	Singleton = NewSingleton;
}
