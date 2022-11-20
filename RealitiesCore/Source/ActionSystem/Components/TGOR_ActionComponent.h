// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "../TGOR_ActionInstance.h"
#include "../TGOR_EventInstance.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "TargetSystem/Interfaces/TGOR_AimReceiverInterface.h"
#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"
#include "TGOR_ActionComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Input;
class UTGOR_Event;
class UTGOR_Action;
class UTGOR_Loadout;
class UTGOR_Performance;
class UTGOR_EquipableAction;
class UTGOR_ActionTask;
class UTGOR_MovementTask;

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


USTRUCT(BlueprintType)
struct FTGOR_ActionSlotSpot
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ActionSlotSpot();

	/** Action type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Action> Type;

	/** Default action type if empty (None for optional) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Action> Obligatory;
};

/////////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionScheduleDelegate, UTGOR_ActionTask*, ActionSlot);

/**
 * TGOR_ActionComponent allows equipment of actions
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UTGOR_ActionComponent : public UTGOR_DimensionComponent,
	public ITGOR_SaveInterface, 
	public ITGOR_SpawnerInterface, 
	public ITGOR_AnimationInterface, 
	public ITGOR_AimReceiverInterface, 
	public ITGOR_AttributeInterface, 
	public ITGOR_RegisterInterface
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

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual TSubclassOf<UTGOR_Performance> GetPerformanceType() const override;
	virtual UTGOR_AnimationComponent* GetAnimationComponent() const override;

	TArray<UTGOR_Modifier*> QueryActiveModifiers_Implementation() const override;

	virtual bool TestAimTarget_Implementation(const FTGOR_AimInstance& Aim) override;
	virtual void ApplyAimTarget_Implementation(const FTGOR_AimInstance& Aim) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Action")
		FActionScheduleDelegate OnActionScheduled;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Action")
		FActionUpdateDelegate OnActionUpdated;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////
	
	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TArray<FTGOR_ActionSlotSpot> ActionSlotSpots;

	/** Mandatory actions in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TArray<TSubclassOf<UTGOR_Action>> SpawnStaticActions;

	/** Slot actions in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TArray<TSubclassOf<UTGOR_Action>> SpawnSlotActions;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UPROPERTY()
		TArray<UTGOR_Action*> StaticActions;

	UPROPERTY()
		TArray<UTGOR_Action*> SlotActions;

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
		bool ScheduleActionType(TSubclassOf<UTGOR_Action> Type, const FTGOR_AimInstance& Aim);

	/** Check whether given action type is currently running */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningActionType(TSubclassOf<UTGOR_Action> Type) const;

	/** Check whether given action type is currently operating */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsOperatingActionType(TSubclassOf<UTGOR_Action> Type) const;

	/** Check whether given action type is supported by the currently running action (used by e.g. abilities running off already running actions) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningActionSupporting(const UTGOR_Action* Action) const;

	/** Get index of currently running action */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		int32 GetCurrentActionSlotIdentifier() const;

	/** Schedule an equipped slot action. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool ScheduleSlotAction(int32 Identifier, const FTGOR_AimInstance& Aim, bool Rerun = false);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get attached action task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_ActionTask* GetCurrentActionOfType(TSubclassOf<UTGOR_ActionTask> Type) const;

	template<typename T> T* GetCurrentAOfType() const
	{
		return Cast<T>(GetCurrentActionOfType(T::StaticClass()));
	}

	/** Get all action tasks of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_ActionTask*> GetActionListOfType(TSubclassOf<UTGOR_ActionTask> Type) const;

	template<typename T> TArray<T*> GetAListOfType() const
	{
		TArray<T*> Output;
		TArray<UTGOR_ActionTask*> Actions = GetActionListOfType(T::StaticClass());
		for (UTGOR_ActionTask* Action : Actions)
		{
			Output.Emplace(Cast<T>(Action));
		}
		return Output;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether any action is currently scheduled. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunning(UTGOR_Action* Action) const;

	/** Whether the given action slot is currently scheduled. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunningAt(int32 Identifier) const;

	/** Whether the given action slot is currently in prepare state. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsPreparingAt(int32 Identifier) const;

	/** Whether the given action slot is currently in operate state. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsOperatingAt(int32 Identifier) const;

	/** Whether the given action slot is currently in finish state. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool IsFinishingAt(int32 Identifier) const;

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

	/** Get more specific information about the state of a given slot. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool CollectDebugInfo(int32 Identifier, const FTGOR_AimInstance& Aim, float Duration, FTGOR_ActionDebugInfo& Info) const;

	/** Get more specific information about the state of all slots. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		void CollectDebugInfos(const FTGOR_AimInstance& Aim, float Duration, TArray<FTGOR_ActionDebugInfo>& Infos) const;

	/** Get callable action slots of given type.
		This does _not_ update context updates before testing. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		TArray<int32> GetCallableActionIdentifiers(TSubclassOf<UTGOR_Action> Type, const FTGOR_AimInstance& Aim, bool CheckCanCall);

	/** Get callable action slots that are subactions of a given action
		This does _not_ update context updates before testing. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		TArray<int32> GetCallableSubactionIdentifiers(UTGOR_Action* Action, const FTGOR_AimInstance& Aim, bool CheckCanCall);

	/** Get whether a given movement can be executed */
	UFUNCTION(BlueprintPure, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		bool CanRunMovement(UTGOR_Movement* Movement) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Animation performance type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Performance> PerformanceType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

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
		bool RunSlotAction(int32 Identifier, const FTGOR_AimInstance& Aim, bool Force, bool Rerun);

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
