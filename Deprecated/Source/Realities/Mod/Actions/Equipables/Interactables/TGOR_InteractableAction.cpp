// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractableAction.h"
#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Targets/TGOR_Target.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_InteractableAction::UTGOR_InteractableAction()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_InteractableAction::InfluenceTarget(UTGOR_ActionComponent* Component, const FTGOR_AimInstance& Aim, const FTGOR_ForceInstance& Forces)
{
	if (IsValid(Component))
	{
		if (Aim.Component.IsValid() && IsValid(Aim.Target))
		{
			UTGOR_InteractableComponent* Interactable = Aim.Target->QueryInteractable(Aim);
			if (IsValid(Interactable))
			{
				// Compute and apply influence
				FTGOR_InfluenceInstance Instance;
				Instance.Forces = Forces;
				Instance.Instigator = Component->GetOwnerPawn();
				Instance.Location = Aim.Target->QueryAimLocation(Aim);
				Instance.Target = Aim.Target;
				Interactable->Influence(Instance);
			}
			else
			{
				ERROR("Target does not have an interactable component.", Error);
			}
		}
		else
		{
			ERROR("No target given", Error);
		}
	}
}