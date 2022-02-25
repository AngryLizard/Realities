// The Gateway of Realities: Planes of Existence.


#include "TGOR_UprightMovement.h"

#include "Realities/MovementSystem/Components/TGOR_MovementComponent.h"

UTGOR_UprightMovement::UTGOR_UprightMovement()
	: Super()
{
	StandingThreshold = 0.8f;
}

bool UTGOR_UprightMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Component, Tick, Space, External))
	{
		return false;
	}

	return (Space.Angular.GetAxisZ() | External.UpVector) > StandingThreshold;
}


float UTGOR_UprightMovement::GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float GroundRatio) const
{
	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementInput& State = Component->GetState();
	return(State.Input | External.UpVector);
}
