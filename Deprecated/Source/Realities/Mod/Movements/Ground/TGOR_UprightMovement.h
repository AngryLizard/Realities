// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/Ground/TGOR_GroundMovement.h"
#include "TGOR_UprightMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////


/**
* TGOR_UprightMovement is a movement mode on the ground
*/
UCLASS()
class REALITIES_API UTGOR_UprightMovement : public UTGOR_GroundMovement
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_UprightMovement();
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	
	/** Threshold (angle) for when character can still stand up [-1, 1], -1 for always and 1 for never */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float StandingThreshold;


	virtual float GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float GroundRatio) const override;

private:

};
