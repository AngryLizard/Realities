// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Event.h"
#include "TGOR_IndexEvent.generated.h"

/**
 *
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_IndexEvent : public UTGOR_Event
{
	GENERATED_BODY()

public:
	UTGOR_IndexEvent() {};
	virtual void Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params) override;

	/** Calls event on an actioncomponent with index params */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CallWithIndexParams(UTGOR_ActionComponent* ActionComponent, int32 Index);

	/** Execute this event on a given action slot. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnIndexCall(UTGOR_ActionTask* ActionSlot, int32 Index);
};
