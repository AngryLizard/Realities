// The Gateway of Realities: Planes of Existence.

#include "TGOR_ValueEvent.h"
#include "../../Components/TGOR_ActionComponent.h"

void UTGOR_ValueEvent::CallWithValueParams(UTGOR_ActionComponent* ActionComponent, float Value)
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->CallEvent(this, FTGOR_EventInstance::Create<FTGOR_EventParamsValue>(Value));
	}
}

void UTGOR_ValueEvent::Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params)
{
	Super::Call(ActionSlot, Params);
	const FTGOR_EventParamsValue* Ptr = Params.Get<FTGOR_EventParamsValue>();
	if (Ptr)
	{
		OnValueCall(ActionSlot, Ptr->Value);
	}
}