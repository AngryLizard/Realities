// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "InventorySystem/TGOR_ItemInstance.h"
#include "../TGOR_ActionInstance.h"

#include "AttributeSystem/Content/TGOR_Modifier.h"
#include "TGOR_Action.generated.h"


////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ActionTask;
class UTGOR_Animation;
class UTGOR_Matter;

class UTGOR_Input;
class UTGOR_Event;
class UTGOR_Target;
class UTGOR_Movement;

////////////////////////////////////////////// ENUMS //////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_ClientAuthority : uint8
{
	/** Only server definitely changes action state. */
	NoAuthority,
	/** Autonomous client can skip operate state. This negates the effects of NoForward in NetworkNotify for operate state. */
	SkipOperate,
	/** Autonomouse client can skip any running state. This negates the effects of NoForward in NetworkNotify for all ticked states. */
	SkipAll
};

UENUM(BlueprintType)
enum class ETGOR_NotifyReplication : uint8
{
	/** Clients don't change state themselves but wait for server update instead (recommended for long-term running actions) */
	NoForward,
	/** Clients don't terminate action on false condition, but they keep checking it (recommended for default).
	*	Server update later tells the client whether to actually terminate the action or not.
	*	While condition is false, no action function is called. */
	NoValidation,
	/** Clients nominally terminate action on false condition (recommended for client-heavy actions with quick response times).
	*	If the server update then tells the client the action should still be running it is scheduled anew.
	*	If the server sends a termination signal the action is still also terminated on the client. */
	Independent
};

/**
 *
 */
UENUM(BlueprintType)
enum class ETGOR_AimCheckEnumeration : uint8
{
	/* Don't check aim at all while running */
	None,
	/* Check current aim target only */
	Current,
	/* Check all targets we can run this action on */
	CanCall,
	/* Check all targets */
	All
};

/**
 *
 */
UENUM(BlueprintType)
enum class ETGOR_AimIgnoreEnumeration : uint8
{
	/* Don't check targets at all */
	None,
	/* Allow all targets in insertion list */
	All,
	/* Allow only targets on others */
	IgnoreSelf,
	/* Allow only target on self */
	OnlySelf
};

/**
 *
 */
UENUM(BlueprintType)
enum class ETGOR_AimDistanceEnumeration : uint8
{
	/* Don't check range */
	NoRange, 
	/* In interaction range */
	InRange, 
	/* In reach */
	InReach,
	/* Out reach but in interaction range */
	OutReach
};

UENUM(BlueprintType)
enum class ETGOR_InputTrigger : uint8
{
	/** Trigger action on input, don't change input state */
	TriggerNoState,
	/** Trigger action on input and change input state */
	TriggerWithState,
	/** Don't trigger on input, only change input state */
	StateOnly
};

/**
* TGOR_Action handles any kind of replicated actions
*/
UCLASS()
class ACTIONSYSTEM_API UTGOR_Action : public UTGOR_Modifier
{
	GENERATED_BODY()

	friend class SGATGORNode_ActionParamPin;
	friend class SGATGORNode_ActionEventPin;
	friend class UTGOR_ActionTask;

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_Action();

	/** Log a message for this given action */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void LogActionMessage(const FString& Message, UTGOR_ActionComponent* Component = nullptr) const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Prints action log for debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Action")
		bool Verbose;

	/** Cooldown for this action in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Action")
		float ScheduleCooldown;

	/** Rules on what control autonomous client has over server */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ClientAuthority ClientAuthority;

	/** Defines how clients handle server updates to the current action state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_NotifyReplication NetworkNotify;

	/** Type of paired item supported by this action (i.e. augments) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Action")
		FTGOR_ItemRestriction SupportedItem;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Aim check rule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_AimCheckEnumeration AimCheck;

	/** Aim ignore rule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_AimIgnoreEnumeration AimTarget;

	/** Aim range rule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_AimDistanceEnumeration AimRange;

	/** Multiple of owner radius/height for reach distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "AimRange == ETGOR_AimDistanceEnumeration::InReach || AimRange == ETGOR_AimDistanceEnumeration::OutReach"))
		float AimReachDistance = 4.0f;

	/** Multiple of owner radius/height for range distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "AimRange == ETGOR_AimDistanceEnumeration::InRange || AimRange == ETGOR_AimDistanceEnumeration::OutReach"))
		float AimRangeDistance = 7.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_ActionTask> TaskType;

	/** Creates an action task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Instance", Meta = (Keywords = "C++"))
		UTGOR_ActionTask* CreateActionTask(UTGOR_ActionComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether autonomous client can skip certain action states (aka action state is sent with payload data) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool HasClientAuthority() const;

	/** Whether this action requires a target */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool HasTargetCondition() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Automatically trigger if valid condition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool AutoTrigger;

	/** Max time for equip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR System")
		float MaxPrepareTime;

	/** Max time for unequip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR System")
		float MaxFinishTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_ActionTask* ActionTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_ActionTask* ActionTask);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Inputs captured by this action, triggers action on input if checked */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName= "Captured Inputs"))
		TMap<TSubclassOf<UTGOR_Input>, ETGOR_InputTrigger> InputInsertions;
	DECL_INSERTION(InputInsertions);

	/** Event captured by this action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Captured Events"))
		TArray<TSubclassOf<UTGOR_Event>> EventInsertions;
	DECL_INSERTION(EventInsertions);

	/** Supported subactions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Supported Actions"))
		TArray<TSubclassOf<UTGOR_Action>> ChildInsertions;
	DECL_INSERTION(ChildInsertions);

	/** Aimed targets for this action to be called */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Supported Targets"))
		TSet<TSubclassOf<UTGOR_Target>> TargetInsertions;
	DECL_INSERTION(TargetInsertions);

	/** Movement modes that support this action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Supported Movements"))
		TSet<TSubclassOf<UTGOR_Movement>> MovementInsertions;
	DECL_INSERTION(MovementInsertions);

	/** Movement to be running during preparation state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Prepare Movement"))
		TSubclassOf<UTGOR_Movement> PrepareMovementInsertion;
	DECL_INSERTION(PrepareMovementInsertion);

	/** Movement to be running during operate state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Operate Movement"))
		TSubclassOf<UTGOR_Movement> OperateMovementInsertion;
	DECL_INSERTION(OperateMovementInsertion);

	/** Movement to be running during finish state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion", Meta = (DisplayName = "Finish Movement"))
		TSubclassOf<UTGOR_Movement> FinishMovementInsertion;
	DECL_INSERTION(FinishMovementInsertion);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;


protected:
private:

};
