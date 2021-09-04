// TGOR (C) // CHECKED //
#pragma once

#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "CoreSystem/Actors/TGOR_Actor.h"
#include "TGOR_DimensionActor.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_IdentityComponent;

/**
*	TGOR_DimensionActor provides functionality for actors that interact with the dimension system.
*	WARNING: Only active in loading phase. Assemble functions need to be called manually when spawned dynamically.
*/
UCLASS()
class DIMENSIONSYSTEM_API ATGOR_DimensionActor : public ATGOR_Actor, public ITGOR_DimensionInterface
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

public:
	FORCEINLINE UTGOR_IdentityComponent* GetIdentity() const { return IdentityComponent; }

protected:

private:
	
};
