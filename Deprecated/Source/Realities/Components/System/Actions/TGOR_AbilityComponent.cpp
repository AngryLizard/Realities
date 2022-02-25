// The Gateway of Realities: Planes of Existence.


#include "TGOR_AbilityComponent.h"

#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"

#include "Realities/Mod/Actions/Equipables/Abilities/Jumps/TGOR_JumpAction.h"
#include "Realities/Mod/Actions/Equipables/Abilities/Sprints/TGOR_SprintAction.h"
#include "Realities/Mod/Actions/Equipables/Abilities/Traversal/TGOR_TraversalAction.h"


UTGOR_AbilityComponent::UTGOR_AbilityComponent()
	: Super()
{
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_JumpAction::StaticClass()));
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_SprintAction::StaticClass()));
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_TraversalAction::StaticClass()));

	AnimationSlotName = "Ability";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
