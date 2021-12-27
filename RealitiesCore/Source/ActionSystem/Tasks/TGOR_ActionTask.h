// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "../TGOR_ActionInstance.h"
#include "InventorySystem/Storage/TGOR_StorageInstance.h"

#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"
#include "TGOR_ActionTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ArmatureComponent;
class UTGOR_MovementComponent;
class ATGOR_CreatureCharacter;
class UTGOR_Animation;
class UTGOR_Matter;

class UTGOR_Input;
class UTGOR_Event;

UENUM(BlueprintType)
enum class ETGOR_StateEnumeration : uint8
{
	KeepState,
	NextState
};

/**
*
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_ActionTaskIdentifier
{
	GENERATED_USTRUCT_BODY();

	/** Action content */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		UTGOR_Action* Content = nullptr;

	/** Owning action component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		UTGOR_ActionComponent* Component = nullptr;

	/** Slot this action resides in */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		int32 Slot = -1;
};


/**
* TGOR_Action handles any kind of replicated actions
*/
UCLASS(Blueprintable)
class ACTIONSYSTEM_API UTGOR_ActionTask : public UTGOR_AnimatedTask
{
	GENERATED_BODY()

		friend class UTGOR_ActionComponent;
		friend class UTGOR_Action;

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ActionTask();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual TScriptInterface<ITGOR_AnimationInterface> GetAnimationOwner() const override;

	/** Log a message for this given action */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void LogActionMessage(const FString& Domain, const FString& Message);

	/** Returns whether action can be called in this context (correct slot, cooldown etc) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		virtual bool CanCall() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual void Initialise();
	virtual bool Condition() const;
	virtual void PrepareStart();
	virtual bool PrepareState(float Progress, float Deltatime);
	virtual void OperateStart();
	virtual bool OperateState(float Progress, float Deltatime);
	virtual void FinishStart();
	virtual bool FinishState(float Progress, float Deltatime);
	virtual void Interrupt();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnInitialise();

	/** Called on all tasks to update context information (i.e. to cache values for CanCall).
		This is called before every update call if this action is scheduled. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnContext();

	/** Called when during a context change the aim target has been modified significantly. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnTarget();

	/** Condition if action can be started.
		If false user can neither schedule this action
		nor will it be visible for players.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnCondition(ETGOR_ValidEnumeration& Branches) const;

	/** Called once when action starts, followed by PrepareState. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnPrepareStart();

	/** This state will be run either for MaxPrepareTime seconds or until
		NextState is returned. Then transitions into OperateStart.
		Usage example: Playing animations like equipping weapons. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnPrepare(float Progress, float Deltatime, ETGOR_StateEnumeration& Branches);

	/** Called once after transitioning from PrepareState, followed by OperateState. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnOperateStart();

	/** This state will be run indefinitely until NextState is returned.
		Do stuff here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnOperate(float Time, float Deltatime, ETGOR_StateEnumeration& Branches);

	/** Called once after transitioning from OperateState followed by FinishState. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnFinishStart();

	/** This state will be run for MaxFinishTime seconds to then
		transition into Interrupt.
		Usage example: Playing animations like unequipping weapons. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnFinish(float Progress, float Deltatime, ETGOR_StateEnumeration& Branches);

	/** Always called when action is stopped/descheduled, even if invariant is false.
		Used for cleanup after action call.
		Usage example: Stopping sounds/particle effects. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnInterrupt();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets assigned action content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_Action* GetAction() const;

	/** Gets assigned action component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_ActionComponent* GetOwnerComponent() const;

	/** Gets assigned action slot index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		int32 GetSlotIndex() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets duration since last call and current time */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		float GetTimeSinceLastCall() const;

	/** Returns duration since last ActionState changed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		float GetDuration(const FTGOR_ActionState& State) const;

	/** Returns whether this storage is currently running in the given state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningIn(const FTGOR_ActionState& State) const;

	/** Returns whether this storage is currently running in its ActionComponent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunning() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets this action's slot item */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* GetCurrentItem() const;

	/** Swaps an item with this action's slot item */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* SwapWithCurrentItem(UTGOR_ItemStorage* ItemStorage);

	/** Takes this action's slot item */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* TakeCurrentItem();

	/** Checks whether given quantity of matter is available to be depleted */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool HasMatter(UTGOR_Matter* Matter, int32 Quantity) const;

	/** Remove given quantity of matter from current item, removed item if empty, returns removed quantity */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		int32 DepleteMatter(UTGOR_Matter* Matter, int32 Quantity);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get currently targeted component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UActorComponent* GetAimedComponent() const;

	/** Get currently targeted content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_Target* GetAimedTarget() const;

	/** Get currently targeted location */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		FVector GetAimedLocation(bool Sticky) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UFUNCTION()
		bool HasNoCooldown() const;

	UFUNCTION()
		bool HasValidTarget() const;

	UFUNCTION()
		bool HasValidMovement() const;

	UFUNCTION()
		bool IsInRange() const;

	/** Checks current aim whether target is in current action insertion list and in range as well as current movement mode. Used by default PreCondition. */
	UFUNCTION()
		bool CheckOwnerState() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Populate with debug information about the current action state */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool CollectDebugInfo(float LogDuration, FTGOR_ActionDebugInfo& DebugInfo) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Update values in this task that depend on outside context (cache for CanCall and subsequent schedule) */
	void Context(const FTGOR_AimInstance& Aim);

	/** Prepare trigger structure for Update calls */
	void Prepare(FTGOR_ActionState& State);

	/** Updates this action state, deschedules itself if invariance becomes false */
	bool Update(FTGOR_ActionState& State, float Deltatime);

	/** Forwards a state to a given state, calling obligatory events in between */
	bool Forward(FTGOR_ActionState& State, ETGOR_ActionStateEnumeration TargetState);

	/** Interrupt this action and clear state */
	void Finish(FTGOR_ActionState& State);

	/** Forward serverside depending on ClientAuthority */
	void Integrate(FTGOR_ActionState& State, const FTGOR_ActionState& TargetState);

	/** Tick animation */
	virtual void Animate(float Deltatime);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called every frame to update animation (multiple action ticks can happen per animation frame). */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnAnimate(float Deltatime);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Time when this action was last called */
	UPROPERTY(Replicated)
		FTGOR_Time LastCallTimestamp;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Update input value, return whether input has been handled */
	bool UpdateInput(TSubclassOf<UTGOR_Input> InputType, float Value, bool AllowTrigger);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called when an input changed threshold (only meaningfully returns results on local client) */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void OnInputChanged(UTGOR_Input* Input, float Value);

	/** Returns whether any input if given type is currently active (only meaningfully returns results on local client) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsActiveInput(TSubclassOf<UTGOR_Input> InputType) const;

	/** Returns highest input value of given type (only meaningfully returns results local client) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		float GetInputValue(TSubclassOf<UTGOR_Input> InputType) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Current input states */
	UPROPERTY()
		FTGOR_ActionInputs Inputs;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owning action component */
	UPROPERTY(ReplicatedUsing = RepNotifyIdentifier, Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_ActionTaskIdentifier Identifier;

	/** Initialise parenting */
	UFUNCTION()
		void RepNotifyIdentifier()
		{
			Initialise();
		}

	/** To this task relevant movement component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		TWeakObjectPtr<UTGOR_MovementComponent> MovementComponent;

	/** To this task relevant pilot component */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		TWeakObjectPtr<UTGOR_ArmatureComponent> PilotComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Logging data if action is verbose */
	UPROPERTY()
		TArray<FTGOR_ActionLog> DebugLogs;

	/** Last record aim target */
	UPROPERTY()
		FTGOR_AimInstance LastAim;


protected:
private:

};
