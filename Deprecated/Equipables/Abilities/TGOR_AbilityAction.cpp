// The Gateway of Realities: Planes of Existence.


#include "TGOR_AbilityAction.h"
#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Mod/Unlocks/Movements/TGOR_Movement.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_AbilityAction::UTGOR_AbilityAction()
	: Super()
{
	MaxPrepareTime = 1.0f;
}

FTGOR_MovementSpace UTGOR_AbilityAction::ComputeMovementSpace(UTGOR_MovementComponent* Component) const
{
	FTGOR_MovementSpace Space;
	if (IsValid(Component))
	{
		return Component->ComputeSpace();
	}
	return Space;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
