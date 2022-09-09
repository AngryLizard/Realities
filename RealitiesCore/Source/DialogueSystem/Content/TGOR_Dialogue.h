// TGOR (C) // CHECKED //
#pragma once

#include "../TGOR_DialogueInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Dialogue.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Attribute;
class UTGOR_Animation;
class UTGOR_DialogueTask;
class UTGOR_DialogueComponent;


UENUM(BlueprintType)
enum class ETGOR_DialogueEnumeration : uint8
{
	// Standard dialogue behaviour (TODO: Actually use this for something)
	Standard
};

/**
*UTGOR_Dialogue is a dialogue the TGOR_DialogueComponent can play
*/
UCLASS()
class DIALOGUESYSTEM_API UTGOR_Dialogue : public UTGOR_SpawnModule
{
	GENERATED_BODY()

	friend class UTGOR_DialogueTask;

public:
	
	UTGOR_Dialogue();

	////////////////////////////////////////////////////////////////////////////////////////////////////
		
	/** Mode of this dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		ETGOR_DialogueEnumeration Mode = ETGOR_DialogueEnumeration::Standard;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_DialogueTask> TaskType;

	/** Creates an dialogue task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Instance", Meta = (Keywords = "C++"))
		UTGOR_DialogueTask* CreateDialogueTask(UTGOR_DialogueComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_DialogueTask* Task);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_DialogueTask* Task);

		
protected:

private:

};
