// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/Ground/TGOR_UprightMovement.h"
#include "TGOR_WalkingMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* TGOR_GroundMovement is a movement mode on the ground
*/
UCLASS()
class REALITIES_API UTGOR_WalkingMovement : public UTGOR_UprightMovement
{
	GENERATED_BODY()

public:
	
	UTGOR_WalkingMovement();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max speed for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumSpeed;

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

	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	float ComputeDirectionRatio(float ForwardRatio, float SpeedRatio, float LockTurn) const;
};
