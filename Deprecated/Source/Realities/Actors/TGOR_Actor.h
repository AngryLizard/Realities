// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/Actor.h"
#include "TGOR_Actor.generated.h"

/**
* TGOR_Actor provides basic resource retreiving functionality, 
* should be inherited by any actor if possible.
*/
UCLASS()
class REALITIES_API ATGOR_Actor : public AActor, public ITGOR_SingletonInterface
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Actor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** */

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
};
