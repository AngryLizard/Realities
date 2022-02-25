// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/Ground/TGOR_GroundMovement.h"
#include "TGOR_HoverMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* UTGOR_HoverMovement is a movement mode that hovers on the ground
*/
UCLASS()
class REALITIES_API UTGOR_HoverMovement : public UTGOR_GroundMovement
{
	GENERATED_BODY()

public:
	
	UTGOR_HoverMovement();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max velocity for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumVelocity;
	
	/** Max rotation angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumAngular;

	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	virtual float GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementContact& Contact, float GroundRatio) const override;
	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const override;

private:
};
