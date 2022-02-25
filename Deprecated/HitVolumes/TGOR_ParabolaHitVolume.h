// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Actors/Combat/HitVolumes/TGOR_FunctionHitVolume.h"
#include "TGOR_ParabolaHitVolume.generated.h"

/**
 * ATGOR_ParabolaHitVolume defines hitvolumes that form a parabola
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_ParabolaHitVolume : public ATGOR_FunctionHitVolume
{
	GENERATED_BODY()
	
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_ParabolaHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/**  */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Parabola dropoff time */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float DropOffTime;

	/** Parabola downwards acceleration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float DownAcceleration;

	/** Parabola downwards acceleration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float StartVelocity;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	virtual void Setup(FTGOR_FunctionSegment& Segment) override;
	virtual FVector Function(float X, const FTGOR_FunctionSegment& Segment, float Time) override;

private:
	
	// ax^2
	float Parabola(float Strength, float X);
	
};
