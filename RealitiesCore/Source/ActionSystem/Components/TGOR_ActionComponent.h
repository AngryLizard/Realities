// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "../TGOR_ActionInstance.h"
#include "../TGOR_EventInstance.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "InventorySystem/Components/TGOR_ItemRegisterComponent.h"
#include "TGOR_ActionComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Input;
class UTGOR_Event;
class UTGOR_Action;
class UTGOR_Loadout;
class UTGOR_Performance;
class UTGOR_EquipableAction;
class UTGOR_ActionTask;

///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_ActionTrigger
{
	GENERATED_BODY()
		FTGOR_ActionTrigger();
	FTGOR_ActionTrigger(float Value, TSubclassOf<UTGOR_Action> Trigger);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Action> Trigger;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionScheduleDelegate, UTGOR_ActionTask*, ActionSlot);

/**
 * TGOR_ActionComponent allows equipment of actions
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UTGOR_ActionComponent : public UTGOR_ItemRegisterComponent, public ITGOR_SaveInterface, public ITGOR_SpawnerInterface, public ITGOR_AnimationInterface
{
	GENERATED_BODY()

	friend class UTGOR_Action;
	friend class UTGOR_ActionStorage;
	friend class UTGOR_InteractionQuery;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ActionComponent();

	virtual void DestroyComponent(bool bPromoteChildren) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual bool CanStoreItemAt(int32 Identifier, UTGOR_ItemStorage* Storage) const override;
	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;

	virtual TSubclassOf<UTGOR_Performance> GetPerformanceType() const override;
	virtual UTGOR_AnimationComponent* GetAnimationComponent() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Action")
		FActionScheduleDelegate OnActionScheduled;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Action")
		FActionUpdateDelegate OnActionUpdated;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////
	
	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Loadout type this action spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Loadout> SpawnLoadout;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Prints action log for debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Action")
		bool Verbose;
		
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Forceably replicates the owning actor immediately. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void ForceReplication();


	/** Schedule last active action of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool ScheduleActionType(TSubclassOf<UTGOR_Action> Type);

	/** Check whether given action type is currently running */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningActionType(TSubclassOf<UTGOR_Action> Type) const;

	/** Check whether given action type is supported by the currently running action (used by e.g. abilities running off already running actions) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningActionSupporting(const UTGOR_Action* Action) const;

	/** Get index of currently running action */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		int32 GetCurrentActionSlotIdentifier() const;

	/** Schedule an equipped slot action. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool ScheduleSlotAction(int32 Identifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether any action is currently scheduled. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunning(UTGOR_Action* Action) const;

	/** Whether the given action slot is currently scheduled. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningAt(int32 Identifier) const;

	/** Whether any action slot it currently running. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningAny() const;

	/** Check whether currently running action is in progress (prepare/finish) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool HasProgress() const;

	/** Get progress in percentage (prepare/finish) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		float GetProgress() const;

	/** Get action on given slot. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_Action* GetAction(int32 Identifier) const;

	/** Get more specific information about the state of all slots. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CollectDebugInfo(float Duration, TArray<FTGOR_ActionDebugInfo>& Infos) const;

	/** Get callable action slots of given type.
		This does _not_ update context updates before testing. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		TArray<int32> GetCallableActionIdentifiers(TSubclassOf<UTGOR_Action> Type, bool CheckCanCall);

	/** Get callable action slots that are subactions of a given action
		This does _not_ update context updates before testing. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		TArray<int32> GetCallableSubactionIdentifiers(UTGOR_Action* Action, bool CheckCanCall);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Animation performance type */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Performance> PerformanceType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Play animation on this component. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void PlayAnimation(TSubclassOf<UTGOR_Animation> InAnimation);

	/** Get world time seconds */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		FTGOR_Time GetServerTime(float Delay);

	/** Get client ping */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		float GetClientPing();

	/** Retreive owner net role */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void GetNetOccupation(ETGOR_NetOccupation& Branches);

	/** Check if running on server */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsServer() const;

	/** Check if running on server */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ServerBranch(ETGOR_BoolEnumeration& Branches);

	/** Check if running locally */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsLocallyControlled() const;

	/** Check if running locally */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void LocalBranch(ETGOR_BoolEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns whether we can currently call a given event on the given action */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool CanCallEvent(UTGOR_Event* Event) const;

	/** Calls an event on the currently running action if supported */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CallEvent(UTGOR_Event* Event, const FTGOR_EventInstance& Params);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/**  */
	UFUNCTION(Reliable, Server, WithValidation)
		void CallEventServer(UTGOR_Event* Event, FTGOR_EventInstance Params);
	virtual void CallEventServer_Implementation(UTGOR_Event* Event, FTGOR_EventInstance Params);
	virtual bool CallEventServer_Validate(UTGOR_Event* Event, FTGOR_EventInstance Params);

	/**  */
	UFUNCTION(Reliable, NetMulticast)
		void CallEventClient(UTGOR_Event* Event, FTGOR_EventInstance Params);
	virtual void CallEventClient_Implementation(UTGOR_Event* Event, FTGOR_EventInstance Params);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned containers. */
	UPROPERTY(Replicated)
		TArray<UTGOR_ActionTask*> ActionSlots;
	
	/** Currently scheduled action */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyActionState)
		FTGOR_ActionState ActionState;

	/** Fast forward action on replication */
	UFUNCTION()
		void RepNotifyActionState(const FTGOR_ActionState& Old);

	/** Adapts the current action state to given container. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void FastForwardState(const FTGOR_ActionState& TargetState);


	/** Run an equipped slot action. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		bool RunSlotAction(int32 Identifier, bool Force);

	/** Update state for the server, checks for matching identifier and schedules action on mismatch */
	UFUNCTION(Reliable, Server, WithValidation, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void SendState(FTGOR_ActionState State);
		virtual void SendState_Implementation(FTGOR_ActionState State);
		virtual bool SendState_Validate(FTGOR_ActionState State);


	/** Sets an action slot's timestamp and returns time since last update in seconds. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		float UpdateActionTimestamp();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set input value to compatible actions (or triggers an action if part of trigger). */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void SetInput(TSubclassOf<UTGOR_Input> InputType, float Value);

	/** Update context for all actions using provided aim target. Should be updated before CanCall to get proper action responses. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void UpdateContext(const FTGOR_AimInstance& Aim);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Applies a loadout to this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool ApplyActionSetup(FTGOR_LoadoutInstance Setup);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Serverside action setup information */
	UPROPERTY(ReplicatedUsing = OnReplicateActionSetup, BlueprintReadOnly, Category = "!TGOR Action")
		FTGOR_LoadoutInstance ActionSetup;

	/** Called on each client on replication of current action setup structure. */
	UFUNCTION()
		void OnReplicateActionSetup();

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
	   	
private:

};
