// TGOR (C) // CHECKED //
#pragma once

#include "DialogueSystem/TGOR_DialogueInstance.h"

#include "DialogueSystem/Content/TGOR_Spectacle.h"
#include "TGOR_Activator.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////
class UTGOR_Participant;
class UTGOR_ActivatorTask;


/**
* TGOR_Activator represents an interactable activity during dialogue
*/
UCLASS()
class DIALOGUESYSTEM_API UTGOR_Activator : public UTGOR_Spectacle
{
	GENERATED_BODY()

	friend class UTGOR_ActivatorTask;

public:
	
	UTGOR_Activator();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_ActivatorTask> TaskType;

	/** Creates an dialogue task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dialogue|Instance", Meta = (Keywords = "C++"))
		UTGOR_ActivatorTask* CreateActivatorTask(UTGOR_ActivatorComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_ActivatorTask* Task);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Dialogue", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_ActivatorTask* Task);

};
