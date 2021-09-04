// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesUGC/Mod/TGOR_ModInstance.h"

#include "../TGOR_Singleton.h"
#include "../Interfaces/TGOR_SingletonInterface.h"
#include "RealitiesUGC/Mod/TGOR_ModSetup.h"
#include "TGOR_Setup.generated.h"

class UTGOR_Data;

UCLASS(Blueprintable)
class CORESYSTEM_API UTGOR_Setup : public UTGOR_ModSetup, public ITGOR_SingletonInterface
{
	GENERATED_BODY()
public:
	UTGOR_Setup();
	virtual bool Attempt_Implementation(bool IsServer) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Creates (or finds if already exists) and returns data */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Data* AddData(TSubclassOf<UTGOR_Data> Type);
		template<typename T> T* AddData()
		{
			return Cast<T>(AddData(T::StaticClass()));
		}
};
