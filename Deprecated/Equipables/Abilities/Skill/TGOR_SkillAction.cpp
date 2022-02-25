// The Gateway of Realities: Planes of Existence.

#include "TGOR_SkillAction.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"


UTGOR_SkillAction::UTGOR_SkillAction()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
bool UTGOR_SkillAction::Precondition(UTGOR_ActionComponent* Component) const
{
	UTGOR_ActionComponent* Equipment = Component->GetOwnerComponent<UTGOR_ActionComponent>();
	if (IsValid(Equipment))
	{
		if (Equipment->IsRunningActionSupporting(this))
		{
			return Super::Precondition(Component);
		}
	}
	return false;
}

bool UTGOR_SkillAction::Invariant(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component) const
{

	return Super::Invariant(Storage, Component);
}
*/