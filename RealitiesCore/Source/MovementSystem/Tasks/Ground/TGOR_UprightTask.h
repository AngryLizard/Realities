// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_GroundTask.h"
#include "TGOR_UprightTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_UprightTask : public UTGOR_GroundTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_UprightTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Crouch factor in terms of max leg length ]0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float CrouchStretch = 0.6f;

	/** Standing factor in terms of max leg length ]0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StandingStretch = 0.9f;

	/** Threshold (angle) for when character can still stand up [-1, 1], -1 for always and 1 for never */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StandingThreshold = 0.8f;

	/** Crouch speed multiplier to maximum speed [0, 1] when fully crouched */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float CrouchSpeedMultiplier = 0.3f;

	/** Compute crouch speed slowdown */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeCrouchSpeedRatio(float GroundRatio) const;


	virtual float GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, const FTGOR_MovementExternal& External) const override;

private:

};
