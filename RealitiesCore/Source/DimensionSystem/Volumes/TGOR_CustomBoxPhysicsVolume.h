// TGOR (C) // CHECKED //
#pragma once

#include "TGOR_BoxPhysicsVolume.h"
#include "TGOR_CustomBoxPhysicsVolume.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////


/**
* .
*/

UCLASS()
class DIMENSIONSYSTEM_API ATGOR_CustomBoxPhysicsVolume : public ATGOR_BoxPhysicsVolume
{
	GENERATED_BODY()
	
public:

	ATGOR_CustomBoxPhysicsVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual FTGOR_PhysicsProperties ComputeSurroundings(const FVector& Location) const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns Gravity direction from a location in local space. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		void ComputeGravityDirection(const FVector& Location, FVector& Direction) const;

	/** Returns volume velocity at a given location in local space (e.h. wind). */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		void ComputeVolumeVelocity(const FVector& Location, FVector& Velocity) const;

	/** Returns volume vorticity at a given location in local space (e.h. tornado). */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		void ComputeVolumeVorticity(const FVector& Location, FVector& Angular) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
