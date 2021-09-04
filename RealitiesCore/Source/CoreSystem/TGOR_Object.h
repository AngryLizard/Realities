// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Interfaces/TGOR_SingletonInterface.h"
#include "RealitiesUtility/Utility/TGOR_WorldObject.h"
#include "TGOR_Object.generated.h"

/**
 * 
 */
UCLASS()
class CORESYSTEM_API UTGOR_Object : public UTGOR_WorldObject, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	
};
