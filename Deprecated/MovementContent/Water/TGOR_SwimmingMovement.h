// TGOR (C) // CHECKED //
#pragma once

#include "TGOR_WaterMovement.h"
#include "TGOR_SwimmingMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_MovementComponent;

/**
* TGOR_GroundMovement is a movement mode on the ground
*/
UCLASS()
class REALITIES_API UTGOR_SwimmingMovement : public UTGOR_WaterMovement
{
	GENERATED_BODY()

public:

	UTGOR_SwimmingMovement();
	virtual void QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max speed for swimming movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeed;

	/** Max strength in swimming direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SwimStrength;

	/** Max rotation angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TurnTorque;

	/** Max rotation angular velocity when rotating around input axis */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SwivelRotation;

	/** Distance to surface to slow down onto */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SurfaceThreshold;

	/** Surface falloff */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SurfaceDistance;


	/** Ratio when character starts moving after turning ([0,..], 0 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LockMovementWithTurning;

	/** Amount of rotation slowing down with speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TorqueSpeedSlowdown;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:

	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const override;

private:

};
