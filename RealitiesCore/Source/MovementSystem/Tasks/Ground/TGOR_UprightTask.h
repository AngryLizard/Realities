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

	/** Crouch speed multiplier to maximum speed [0, 1] when fully crouched */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float CrouchSpeedMultiplier = 0.3f;

	/** Threshold for when we regard this movement to be standing upright [-1, 1] for -1 for always and 1 for never */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StandingThreshold = 0.25f;

	/** Compute crouch speed slowdown */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeCrouchSpeedRatio(float GroundRatio) const;

	/** Attribute threshold for when this movement is valid */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AutonomyThreshold = 0.5f;

	/** Attribute for whether we're currently able to stand upright */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Attribute> AutonomyAttribute;


	virtual float GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether we're currently low enough to be crouching */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsCrouching() const;

	/** Whether we're currently high enough to be standing */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsStanding() const;

	/** Whether we're currently high enough to be falling */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsFalling() const;

private:

};
