// TGOR (C) // CHECKED //
#pragma once

#include "TGOR_GroundMovement.h"
#include "TGOR_GroundPoseMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* TGOR_GroundPoseMovement is a movement mode on the ground that can hold a pose
*/
UCLASS()
class REALITIES_API UTGOR_GroundPoseMovement : public UTGOR_GroundMovement
{
	GENERATED_BODY()

public:

	UTGOR_GroundPoseMovement();
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Axis snap ratio in Z axis in positive (X) and negative (Y) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector2D ZAxisSnap;

	/** Torque applied on input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float RotationTorque;

	/** Applied torque to snap the axis towards durface normals */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SnappingTorque;

	/** Applied torque around surface normal on input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SwayTorque;

	/** Whether character can only move in major axis */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		bool RestrictsAxis;

	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	virtual float GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float GroundRatio) const override;
	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const override;

private:
};
