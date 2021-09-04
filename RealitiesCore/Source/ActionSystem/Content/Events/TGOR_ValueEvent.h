// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Event.h"
#include "TGOR_ValueEvent.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_ValueEvent : public UTGOR_Event
{
	GENERATED_BODY()
	
public:
	UTGOR_ValueEvent() {};
	virtual void Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params) override;

	/** Calls event on an actioncomponent with value params */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CallWithValueParams(UTGOR_ActionComponent* ActionComponent, float Value);

	/** Execute this event on a given action slot. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnValueCall(UTGOR_ActionTask* ActionSlot, float Value);
};
