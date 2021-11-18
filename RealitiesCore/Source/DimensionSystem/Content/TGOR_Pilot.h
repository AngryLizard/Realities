// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Pilot.generated.h"

class UTGOR_PilotTask;

/**
 * 
 */
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_Pilot : public UTGOR_SpawnModule
{
	GENERATED_BODY()

	friend class UTGOR_PilotTask;

public:
	UTGOR_Pilot();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this Pilot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_PilotTask> TaskType;

	/** Creates a pilot task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Instance", Meta = (Keywords = "C++"))
		UTGOR_PilotTask* CreatePilotTask(UTGOR_PilotComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_PilotTask* PilotTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_PilotTask* PilotTask);

};