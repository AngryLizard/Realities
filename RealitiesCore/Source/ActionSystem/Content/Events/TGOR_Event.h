// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../../TGOR_EventInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Event.generated.h"

class UTGOR_ActionTask;
class UTGOR_ActionComponent;

/**
 * 
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_Event : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Event();

	/** Calls event on an actioncomponent with base params */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CallWithBaseParams(UTGOR_ActionComponent* ActionComponent);

	/** Whether this event is relevant for given priority. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRelevant(ETGOR_NetvarEnumeration Priority) const;

	/** Execute this event on a given action slot with params. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		virtual void Call(UTGOR_ActionTask* ActionSlot, const FTGOR_EventInstance& Params);

	/** Execute this event on a given action slot. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnCall(UTGOR_ActionTask* ActionSlot);

public:

	/** Priority used for networking. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		ETGOR_NetvarEnumeration EventPriority;

	/** Whether this event can only be called on the server */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		bool AuthorityOnly;

	/** Whether this event can only be called during operation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		bool OperateOnly;
};
