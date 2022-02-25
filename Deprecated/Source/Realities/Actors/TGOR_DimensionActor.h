// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "TGOR_Actor.h"
#include "TGOR_DimensionActor.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_DimensionData;

/**
*	TGOR_DimensionActor provides functionality for actors that interact with the dimension system.
*	WARNING: Only active in loading phase. Assemble functions need to be called manually when spawned dynamically.
*/
UCLASS()
class REALITIES_API ATGOR_DimensionActor : public ATGOR_Actor, public ITGOR_DimensionInterface
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_DimensionActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Dimension loader */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_IdentityComponent* IdentityComponent;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
