// The Gateway of Realities: Planes of Existence.

#include "TGOR_Event.h"
#include "../../Components/TGOR_ActionComponent.h"

UTGOR_Event::UTGOR_Event()
	: Super(),
	EventPriority(ETGOR_NetvarEnumeration::Client),
	AuthorityOnly(false),
	OperateOnly(false)
{
}

void UTGOR_Event::CallWithBaseParams(UTGOR_ActionComponent* ActionComponent)
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->CallEvent(this, FTGOR_EventInstance::Create<FTGOR_EventParamsBase>());
	}
}

void UTGOR_Event::Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params)
{
	OnCall(ActionSlot);
}

bool UTGOR_Event::IsRelevant(ETGOR_NetvarEnumeration Priority) const
{
	return EventPriority >= Priority;
}
