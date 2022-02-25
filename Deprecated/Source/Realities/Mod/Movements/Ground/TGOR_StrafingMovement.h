// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/Ground/TGOR_UprightMovement.h"
#include "TGOR_StrafingMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* TGOR_StrafingMovement is a movement mode on the ground that always orients the capsule towards the view
*/
UCLASS()
class REALITIES_API UTGOR_StrafingMovement : public UTGOR_UprightMovement
{
	GENERATED_BODY()

public:
	
	UTGOR_StrafingMovement();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max speed for ground movement backward */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumSpeedForward;

	/** Max speed for ground movement sideways */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumSpeedSideways;

	/** Max speed for ground movement backward */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumSpeedBackward;


	/** Ratio when character starts moving after turning ([0,..], 0 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float LockMovementWithTurning;

	/** Rotational slowdown with speed ([0, 1], 1 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float TorqueSpeedSlowdown;


	/** Input torque coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float TurnTorque;



	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Computes maximum speed */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeMaxSpeed(UTGOR_MovementComponent* Component, const FTGOR_MovementContact& Contact, FVector& View) const;

	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
};
