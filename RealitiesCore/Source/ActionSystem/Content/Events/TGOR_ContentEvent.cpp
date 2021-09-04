// The Gateway of Realities: Planes of Existence.

#include "TGOR_ContentEvent.h"
#include "../../Components/TGOR_ActionComponent.h"

void UTGOR_ContentEvent::CallWithContentParams(UTGOR_ActionComponent* ActionComponent, UTGOR_Content* Content)
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->CallEvent(this, FTGOR_EventInstance::Create<FTGOR_EventParamsContent>(Content));
	}
}

void UTGOR_ContentEvent::Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params)
{
	Super::Call(ActionSlot, Params);
	const FTGOR_EventParamsContent* Ptr = Params.Get<FTGOR_EventParamsContent>();
	if (Ptr)
	{
		OnContentCall(ActionSlot, Ptr->Content);
	}
}