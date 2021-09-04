// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Event.h"
#include "TGOR_ContentEvent.generated.h"

class UTGOR_Content;

/**
 *
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_ContentEvent : public UTGOR_Event
{
	GENERATED_BODY()

public:
	UTGOR_ContentEvent() {};
	virtual void Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params) override;

	/** Calls event on an actioncomponent with index params */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CallWithContentParams(UTGOR_ActionComponent* ActionComponent, UTGOR_Content* Content);

	/** Execute this event on a given action slot. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnContentCall(UTGOR_ActionTask* ActionSlot, UTGOR_Content* Content);
};
