// TGOR (C) // CHECKED //
#pragma once

#include "../TGOR_DialogueInstance.h"

#include "DialogueSystem/Content/TGOR_Spectacle.h"
#include "TGOR_Dialogue.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Attribute;
class UTGOR_Trigger;
class UTGOR_Participant;
class UTGOR_DialogueTask;
class UTGOR_DialogueComponent;


UENUM(BlueprintType)
enum class ETGOR_DialogueEnumeration : uint8
{
	// Standard dialogue behaviour (TODO: Actually use this for something)
	Standard
};

UENUM(BlueprintType)
enum class ETGOR_DialogueConditionEnumeration : uint8
{
	Ever UMETA(ToolTip = "Was ever called anywhere in this dimension (not yet supported, currently equivalent to owner)"),
	Owner UMETA(ToolTip = "Was ever called on owner"),
	Recently UMETA(ToolTip = "Was called in current dialogue interaction")
};

/**
*UTGOR_Dialogue is a dialogue the TGOR_DialogueComponent can play
*/
UCLASS()
class DIALOGUESYSTEM_API UTGOR_Dialogue : public UTGOR_Spectacle
{
	GENERATED_BODY()

	friend class UTGOR_DialogueTask;

public:
	
	UTGOR_Dialogue();

	////////////////////////////////////////////////////////////////////////////////////////////////////
		
	/** Mode of this dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dialogue")
		ETGOR_DialogueEnumeration Mode = ETGOR_DialogueEnumeration::Standard;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_DialogueTask> TaskType;

	/** Creates an dialogue task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue|Instance", Meta = (Keywords = "C++"))
		UTGOR_DialogueTask* CreateDialogueTask(UTGOR_DialogueComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_DialogueTask* Task);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_DialogueTask* Task);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Dialogues that had to be called for this dialogue to become available */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TMap<TSubclassOf<UTGOR_Dialogue>, ETGOR_DialogueConditionEnumeration> ConditionInsertions;
	DECL_INSERTION(ConditionInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
