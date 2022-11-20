// TGOR (C) // CHECKED //
#pragma once

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Participant.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ActivatorComponent;
class UTGOR_Activator;


/**
* TGOR_Participant represents an identity that can be targeted in dialogues
*/
UCLASS()
class DIALOGUESYSTEM_API UTGOR_Participant : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	
	UTGOR_Participant();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Permissible actor type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AActor> ActorFilter;

	/** Whether an occupant can be registered as this participant on content level (also gets checked by ParticipantComponent) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interaction", Meta = (Keywords = "C++"))
		bool IsOccupantCompatible(AActor* Actor) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Activators required for this Participant to match */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TMap<TSubclassOf<UTGOR_Activator>, int32> ConditionInsertions;
	DECL_INSERTION(ConditionInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
