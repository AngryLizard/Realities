// TGOR (C) // CHECKED //
#pragma once

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Trigger.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Activator;
class UTGOR_Participant;



/**
* TGOR_Trigger represents a condition on a specific activator represented by a progress value between 0 and 1
*/
UCLASS()
class DIALOGUESYSTEM_API UTGOR_Trigger : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	
	UTGOR_Trigger();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Activators this trigger is compatible with (OR list ordered by priority) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TArray<TSubclassOf<UTGOR_Activator>> ActivatorInsertions;
	DECL_INSERTION(ActivatorInsertions);

	/** Participant roles (AND list). 
		Access is granted to external participants if there are no dominant or switch participants */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dialogue")
		TMap<TSubclassOf<UTGOR_Participant>, ETGOR_TriggerOwnerEnumeration> ParticipantInsertions;
	DECL_INSERTION(ParticipantInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};
