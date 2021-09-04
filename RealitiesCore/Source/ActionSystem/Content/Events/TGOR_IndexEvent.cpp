// The Gateway of Realities: Planes of Existence.

#include "TGOR_IndexEvent.h"
#include "../../Components/TGOR_ActionComponent.h"

void UTGOR_IndexEvent::CallWithIndexParams(UTGOR_ActionComponent* ActionComponent, int32 Index)
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->CallEvent(this, FTGOR_EventInstance::Create<FTGOR_EventParamsIndex>(Index));
	}
}

void UTGOR_IndexEvent::Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params)
{
	Super::Call(ActionSlot, Params);
	const FTGOR_EventParamsIndex* Ptr = Params.Get<FTGOR_EventParamsIndex>();
	if (Ptr)
	{
		OnIndexCall(ActionSlot, Ptr->Index);
	}
}