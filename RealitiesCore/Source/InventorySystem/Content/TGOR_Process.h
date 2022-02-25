// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_ProcessInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Process.generated.h"

class UTGOR_Matter;
class UTGOR_ProcessTask;
class UTGOR_ProcessComponent;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_Process : public UTGOR_SpawnModule
{
	GENERATED_BODY()

		friend class UTGOR_ProcessTask;
	
public:
	UTGOR_Process();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this process */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_ProcessTask> TaskType;

	/** Creates an process task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory|Instance", Meta = (Keywords = "C++"))
		UTGOR_ProcessTask* CreateProcessTask(UTGOR_ProcessComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_ProcessTask* ProcessTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_ProcessTask* ProcessTask);

};
