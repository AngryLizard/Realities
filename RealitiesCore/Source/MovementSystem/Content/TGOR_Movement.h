// TGOR (C) // CHECKED //
#pragma once

#include "../TGOR_MovementInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
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
class MOVEMENTSYSTEM_API UTGOR_Movement : public UTGOR_CoreContent
{
	GENERATED_BODY()

	friend class UTGOR_MovementTask;

public:
	
	UTGOR_Movement();
	virtual void BuildResource() override;
	virtual void PostBuildResource() override;
	virtual bool Validate_Implementation() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fill override lists */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void OverrideMovements(UTGOR_Movement* Override);

	/** Whether angular velocity is affected by collisions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		bool CanRotateOnCollision;
	
	/** Mode of this movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		ETGOR_MovementEnumeration Mode;

	/** Base animation module of this movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Animation> MainAnimation;

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

	/** Static task */
	UPROPERTY(Transient, BlueprintReadOnly)
		UTGOR_MovementTask* StaticTask;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_MovementTask* MovementTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Maximum speed ratio attribute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Attribute> SpeedAttribute;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Check whether this movement is overriden by a given movement */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsOverriddenBy(UTGOR_Movement* Movement) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Which movements override this one */
	UPROPERTY(BlueprintReadWrite)
		TArray<UTGOR_Movement*> OverriddenBy;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Override movement insertions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Movement>> OverrideInsertions;
		DECL_INSERTION(OverrideInsertions);

	/** Attributes supported by this movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Attribute>> AttributeInsertions;
		DECL_INSERTION(AttributeInsertions);

		virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

	
protected:

private:

};
