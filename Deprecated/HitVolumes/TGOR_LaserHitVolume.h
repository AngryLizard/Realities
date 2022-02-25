// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Actors/Combat/HitVolumes/TGOR_HitVolume.h"
#include "TGOR_LaserHitVolume.generated.h"

/**
 * ATGOR_LaserHitVolume defines hitvolumes that act like a laser
 */
UCLASS(Blueprintable)
class REALITIES_API ATGOR_LaserHitVolume : public ATGOR_HitVolume
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_LaserHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Build(const FTGOR_ElementInstance& State, const FVector& Focus) override;
	virtual void Update(const FTGOR_ElementInstance& State, const FVector& Focus) override;
	virtual bool VolumeTick(float DeltaSeconds) override;
	virtual void Terminate(FTGOR_Time TerminationTimestamp) override;

	//////////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////

	/** Moves a volume */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR HitVolume", Meta = (Keywords = "C++"))
		bool RenderVolume(const FVector& From, const FVector& To, const FVector& Normal, float Time);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////
private:

	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Last laser update time */
	UPROPERTY()
		FTGOR_Time DelayedTime;

	/** Last location of laser */
	UPROPERTY()
		FVector DelayedLocation;

protected:

	/** Time laser expires after last update */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float ExpirationTime;

	/** Max speed of laser end point in cm per second (No limit with 0.0) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float MaxLaserVelocity;

	/** Max laser distance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR HitVolume")
		float MaxLaserDistance;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
