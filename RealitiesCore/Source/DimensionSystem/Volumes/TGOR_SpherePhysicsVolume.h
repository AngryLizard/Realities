// TGOR (C) // CHECKED //
#pragma once

#include "Components/SphereComponent.h"

#include "TGOR_PhysicsVolume.h"
#include "TGOR_SpherePhysicsVolume.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////


/**
* .
*/

UCLASS()
class DIMENSIONSYSTEM_API ATGOR_SpherePhysicsVolume : public ATGOR_PhysicsVolume
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_SpherePhysicsVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsInside(const FVector& Location) const override;
	virtual float ComputeVolume() const override;
	virtual FBox ComputeBoundingBox() const override;
	virtual FTGOR_PhysicsProperties ComputeSurroundings(const FVector& Location) const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		USphereComponent* SphereComponent;

public:

	FORCEINLINE USphereComponent* GetSphere() const { return SphereComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute depth for a given location and direction in local space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
	virtual float ComputeDepth(const FVector& Location, const FVector& Direction) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
