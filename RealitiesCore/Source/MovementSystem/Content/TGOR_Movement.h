// TGOR (C) // CHECKED //
#pragma once

#include "../TGOR_MovementInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Movement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Attribute;
class UTGOR_Animation;
class UTGOR_MovementTask;
class UTGOR_MovementComponent;


UENUM(BlueprintType)
enum class ETGOR_MovementEnumeration : uint8
{
	Queued, // Queued movement, all movements above the queue override even with true invariant
	Sticky, // Queued movement, but doesn't get overridden by movements above the queue
	Manual // Needs to be executed by preemption, only disappears once invariant is false
};

/**
* TGOR_Movement is a movement mode the MovementComponent activates and ticks
*/
UCLASS()
class MOVEMENTSYSTEM_API UTGOR_Movement : public UTGOR_SpawnModule
{
	GENERATED_BODY()

	friend class UTGOR_MovementTask;

public:
	
	UTGOR_Movement();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Whether angular velocity is affected by collisions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		bool CanRotateOnCollision;
	
	/** Mode of this movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		ETGOR_MovementEnumeration Mode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_MovementTask> TaskType;

	/** Creates an movement task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Instance", Meta = (Keywords = "C++"))
		UTGOR_MovementTask* CreateMovementTask(UTGOR_MovementComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_MovementTask* ActionTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_MovementTask* MovementTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Maximum speed ratio attribute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Attribute> SpeedAttribute;
		
protected:

private:

};
