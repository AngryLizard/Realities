// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Components/BoxComponent.h"

#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "TGOR_BoxPhysicsVolume.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////


/**
* .
*/

UCLASS()
class REALITIES_API ATGOR_BoxPhysicsVolume : public ATGOR_PhysicsVolume
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_BoxPhysicsVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsInside(const FVector& Location) const override;
	virtual float ComputeVolume() const override;
	virtual FBox ComputeBoundingBox() const override;
	virtual FTGOR_PhysicsProperties ComputeSurroundings(const FVector& Location) const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxComponent;

public:

	FORCEINLINE UBoxComponent* GetBox() const { return BoxComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute depth for a given location and direction in local space */
	UFUNCTION(BlueprintPure, Category = "TGOR Volume", Meta = (Keywords = "C++"))
	virtual float ComputeDepth(const FVector& Location, const FVector& Direction) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
