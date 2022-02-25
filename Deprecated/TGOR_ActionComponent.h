// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Components/Inventory/TGOR_ContainerComponent.h"
#include "Components/System/TGOR_InteractableComponent.h"
#include "Components/System/TGOR_LogComponent.h"
#include "Utility/TGOR_CustomEnumerations.h"
#include "Base/Instances/System/TGOR_ActionInstance.h"

#include "Components/TGOR_Component.h"
#include "TGOR_ActionComponent.generated.h"

class ATGOR_PlayerController;
class ATGOR_Pawn;

#define MAX_LAG_COMPENSATION 0.2f

/**
* TGOR_ActionComponent adds action calling functionality to an actor
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ActionComponent : public UTGOR_Component
{
	GENERATED_BODY()
	
public:
	UTGOR_ActionComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginDestroy() override;

	/** Updates an action */
	UFUNCTION()
		void Update(FTGOR_ActionTrigger& Trigger, int32 Index, float Timestamp);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Prints action log for debugging */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TGOR|Actions")
		bool Verbose;

	/** Amount of seconds to wait until inactive actions are deleted in client */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TGOR|Actions")
		float InactiveTimeout;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get world time seconds */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		float GetServerTime(float Delay);

	/** Get client ping */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		float GetClientPing();

	/** Retreives unique identifier */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		FTGOR_ActionIndex GenerateIdentifier();

	/** Retreive owner net role */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void GetNetOccupation(ETGOR_NetOccupation& Branches);

	/** Check if running locally */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		bool IsLocallyControlled();
	
	/** Check if running on server */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		bool IsServer();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Returns action cooldown */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		float GetActionIndexCooldown(int32 ActionIndex);

	/** Returns action cooldown for any currently scheduled action of type */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		float GetActionCooldown(TSubclassOf<UTGOR_Action> Action);

	/** Adds cooldown to a  interaction trigger */
	UFUNCTION(Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void AddCooldown(const FTGOR_ActionTrigger& Trigger, float Cooldown);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Currently scheduled actions */
	UPROPERTY(Replicated)
		FTGOR_ActionQueue TriggerQueue;

	/** Currently running cooldowns */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Actions")
		TMap<int32, float> CooldownQueue;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Queries scheduled action type or null if not scheduled */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		UTGOR_Action* GetScheduledAction(FTGOR_ActionIndex Identifier);

	/** Queries whether action is scheduled */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		bool IsActionScheduled(FTGOR_ActionIndex Identifier);

	/** Queries whether action is scheduled */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		bool IsAnyActionScheduled(UTGOR_Action* Action);

	/** Queries whether action class is scheduled */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		bool IsAnyActionClassScheduled(TSubclassOf<UTGOR_Action> ActionClass);
	
	/** Schedule action from index */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_ActionIndex ScheduleParamsAction(TSubclassOf<UTGOR_Action> ActionClass, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches);

	/** Schedule action from index */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_ActionIndex ScheduleActionIndex(int32 ActionIndex, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches);

	/** Schedule action from resourceable */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_ActionIndex ScheduleAction(UTGOR_Action* Action, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches);

	/** Schedule action on server */
	UFUNCTION(Reliable, Server, WithValidation, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void RequestRegisterAction(FTGOR_ActionTrigger Trigger);
	virtual void RequestRegisterAction_Implementation(FTGOR_ActionTrigger Trigger);
	virtual bool RequestRegisterAction_Validate(FTGOR_ActionTrigger Trigger);

	/** Schedule action */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void RegisterAction(FTGOR_ActionTrigger Trigger);

	/** Add action to queue */
	UFUNCTION(Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		FTGOR_ActionTrigger& TriggerAction(FTGOR_ActionTrigger& Trigger, float Time);


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Kill an action on all clients (Server only)*/
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void ActionKillAll(TSubclassOf<UTGOR_Action> Action);

	/** Kill an action on all clients (Server only) */
	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void ActionKillNotify(FTGOR_ActionIndex Identifier);
		virtual void ActionKillNotify_Implementation(FTGOR_ActionIndex Identifier);

	/** Kill an action on this client by its queue index */
	UFUNCTION()
		void ActionKill(int32 Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Send values to server reliably */
	UFUNCTION(Reliable, Server, WithValidation, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void SetVariablesReliable(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);
	virtual void SetVariablesReliable_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);
	virtual bool SetVariablesReliable_Validate(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);

	/** Send values to server unreliably */
	UFUNCTION(Unreliable, Server, WithValidation, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void SetVariablesUnreliable(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);
	virtual void SetVariablesUnreliable_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);
	virtual bool SetVariablesUnreliable_Validate(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Notify all actions of a certain type */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ActionNotifyAll(TSubclassOf<UTGOR_Action> Action, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches);

	/** Notify an action */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ActionNotify(const FTGOR_ActionIndex& Identifier, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches);

	/** Notify an action trigger  */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ActionNotifyTrigger(FTGOR_ActionTrigger& Trigger, int32 Index, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches);

	/** Send notify to server */
	UFUNCTION(Reliable, Server, WithValidation, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void RequestActionNotify(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);
	virtual void RequestActionNotify_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);
	virtual bool RequestActionNotify_Validate(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params);

	/** Send back to clients */
	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void CommitActionNotify(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params, float Time);
	virtual void CommitActionNotify_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params, float Time);

	/** Send error notify back to client */
	UFUNCTION(BlueprintCallable, Reliable, Client, Category = "TGOR|Actions", Meta = (Keywords = "C++"))
		void FailActionNotify(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params, float Time);
	virtual void FailActionNotify_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params, float Time);

private:
	// Keeps track of identifiers
	uint8 _counter;
};
