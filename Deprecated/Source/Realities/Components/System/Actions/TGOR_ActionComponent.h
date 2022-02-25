// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Interfaces/TGOR_UnlockInterface.h"
#include "Realities/Components/Inventory/TGOR_ItemRegisterComponent.h"
#include "TGOR_ActionComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_Pawn;
class UTGOR_Input;
class UTGOR_Event;
class UTGOR_AimComponent;
class UTGOR_ActionComponent;
class UTGOR_ContainerComponent;
class UTGOR_MovementComponent;
class UTGOR_EquipableAction;

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
struct FTGOR_ActionSlot
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;
	FTGOR_ActionSlot();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Action* Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Timestamp;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionSlot> : public TStructOpsTypeTraitsBase2<FTGOR_ActionSlot>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType)
struct FTGOR_ActionContainer
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;
	FTGOR_ActionContainer();
	FTGOR_ActionContainer(TSubclassOf<UTGOR_Action> Type);

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<UTGOR_Action> Type;

	UPROPERTY(BlueprintReadOnly)
		FTGOR_SlotIndex LastActive;

	UPROPERTY(BlueprintReadOnly)
		TArray<FTGOR_ActionSlot> Slots;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionContainer> : public TStructOpsTypeTraitsBase2<FTGOR_ActionContainer>
{
	enum
	{
		WithNetSerializer = true
	};
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionScheduleDelegate, UTGOR_Action*, Action);

/**
 * TGOR_ActionComponent allows equipment of actions
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ActionComponent : public UTGOR_ItemRegisterComponent, public ITGOR_SaveInterface, public ITGOR_UnlockInterface
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

	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual bool CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const override;
	virtual int32 GetContainerCapacity(const FTGOR_ContainerIndex& Index) const override;
	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	virtual void UpdateUnlocks_Implementation(UTGOR_UnlockComponent* Component) override;
	virtual bool SupportsContent_Implementation(UTGOR_Content* Content) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "TGOR Action")
		FActionScheduleDelegate OnActionScheduled;

	UPROPERTY(BlueprintAssignable, Category = "TGOR Action")
		FActionUpdateDelegate OnActionUpdated;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////
	
	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Allowed content types (Empty if there are no constraints) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TSet<UTGOR_Content*> Constraints;

	/** Assigned Pawn. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "TGOR Pawn")
		ATGOR_Pawn* Pawn;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns owner pawn. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		ATGOR_Pawn* GetOwnerPawn() const;

	/** Returns owner avatar. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		ATGOR_Avatar* GetOwnerAvatar(ETGOR_BoolEnumeration& Branches);

	/** Builds action queues from creature setup */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void BuildActionQueues(UTGOR_Content* Content);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Computes cooldown of given slot. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		float GetSlotCooldown(const FTGOR_SlotIndex& Index) const;

	/** Get index of a container of given type */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		FTGOR_ContainerIndex GetContainerTypeIndex(TSubclassOf<UTGOR_Action> Type) const;

	/** Schedule last active action of a container of given type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool ScheduleContainerType(TSubclassOf<UTGOR_Action> Type);

	/** Get number of slots in a given container */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		int32 GetContainerSize(const FTGOR_ContainerIndex& Index) const;

	/** Get index of last used slot in given container (or first slot if none was used yet) */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		FTGOR_SlotIndex GetContainerLastSlot(const FTGOR_ContainerIndex& Index) const;

	/** Get index of currently running action */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		FTGOR_SlotIndex GetCurrentActionSlot() const;

	/** Schedule last active action of a given container */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool ScheduleContainer(const FTGOR_ContainerIndex& Index);

	/** Schedule an equipped slot action. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool ScheduleSlotAction(const FTGOR_SlotIndex& Index);

	/** Set a slot active */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SetSlotActive(const FTGOR_SlotIndex& Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Add action to this component, returns SlotID. (Call on server only) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_SlotIndex AddSlotAction(UTGOR_Action* Action, ETGOR_ComputeEnumeration& Branches);

	/** Set action to a specific slot of this component. (Call on server only) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void SetSlotAction(const FTGOR_SlotIndex& Index, UTGOR_Action* Action, ETGOR_ComputeEnumeration& Branches);
	
	/** Remove action from this component, returns SlotID. (Call on server only) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FTGOR_SlotIndex RemoveSlotAction(UTGOR_Action* Action, ETGOR_ComputeEnumeration& Branches);

	/** Remove all actions from this component of the given type. (Call on server only) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void RemoveAllSlotActions(TSubclassOf<UTGOR_Action> Type);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get action on given slot. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_Action* GetAction(const FTGOR_SlotIndex& Index) const;

	/** Quality of life function that schedules an event on current action. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ScheduleEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_FetchEnumeration& Branches);

	/** Quality of life function that both schedules an event on current action and returns a module to set values for. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (DeterminesOutputType = "Type", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_Module* ScheduleEventParams(TSubclassOf<UTGOR_Event> Event, TSubclassOf<UTGOR_Module> Type, ETGOR_FetchEnumeration& Branches);

	/** Whether any action is currently scheduled. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool IsRunning(UTGOR_ActionStorage* Storage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Animation slot name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Animation")
		FName AnimationSlotName;

	/** Current active animation */
	UPROPERTY(Transient)
		UTGOR_Animation* Animation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Play animation on this component. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void PlayAnimation(TSubclassOf<UTGOR_Animation> InAnimation);

	/** Get current animation of a given type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (DeterminesOutputType = "Type", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_Animation* GetAnimation(TSubclassOf<UTGOR_Animation> Type, ETGOR_FetchEnumeration& Branches);

	/** Get world time seconds */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		FTGOR_Time GetServerTime(float Delay);

	/** Get client ping */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		float GetClientPing();

	/** Retreive owner net role */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void GetNetOccupation(ETGOR_NetOccupation& Branches);

	/** Check if running on server */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool IsServer();

	/** Check if running on server */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void ServerBranch(ETGOR_BoolEnumeration& Branches);

	/** Check if running locally */
	UFUNCTION(BlueprintPure, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool IsLocallyControlled() const;

	/** Check if running locally */
	UFUNCTION(BlueprintCallable, Category = "TGOR Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void LocalBranch(ETGOR_BoolEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Prints action log for debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Action")
		bool Verbose;

	/** Currently assigned containers. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = RepNotifyActionContainers, Category = "TGOR Action")
		TArray<FTGOR_ActionContainer> ActionContainers;

	/**  */
	UFUNCTION()
		void RepNotifyActionContainers();
	
	/** Currently scheduled action */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyActionState)
		FTGOR_ActionState ActionState;

	/** Fast forward action on replication */
	UFUNCTION()
		void RepNotifyActionState(FTGOR_ActionState Old);

	/** Adapts current slot data to given container. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void AdaptToContainer(const FTGOR_ActionState& Container, UTGOR_ActionStorage* Previous);

	/** Run an equipped slot action. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		bool RunSlotAction(const FTGOR_SlotIndex& Index, bool Force);


	/** Schedule action on server */
	UFUNCTION(Reliable, Server, WithValidation, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void ScheduleActionServer(FTGOR_SlotIndex SlotIndex);
		virtual void ScheduleActionServer_Implementation(FTGOR_SlotIndex SlotIndex);
		virtual bool ScheduleActionServer_Validate(FTGOR_SlotIndex SlotIndex);

	/** Correct action on all clients */
	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void AdjustClientActionTrigger(FTGOR_ActionState Container);
		virtual void AdjustClientActionTrigger_Implementation(FTGOR_ActionState Container);

	/** Send values to server reliably */
	UFUNCTION(Reliable, Server, WithValidation, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SendVariablesReliable(FTGOR_ActionPayload Params);
		virtual void SendVariablesReliable_Implementation(FTGOR_ActionPayload Params);
		virtual bool SendVariablesReliable_Validate(FTGOR_ActionPayload Params);

	/** Send values to server unreliably */
	UFUNCTION(Unreliable, Server, WithValidation, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SendVariablesUnreliable(FTGOR_ActionPayload Params);
		virtual void SendVariablesUnreliable_Implementation(FTGOR_ActionPayload Params);
		virtual bool SendVariablesUnreliable_Validate(FTGOR_ActionPayload Params);

	/** Sets an action slot's timestamp and returns time since last update in seconds. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		float UpdateActionTimestamp(const FTGOR_SlotIndex& Index);

	/** Sets an action container's last active slot. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SetLastActiveActionSlot(const FTGOR_SlotIndex& Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Input trigger types. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Action")
		TMap<TSubclassOf<UTGOR_Input>, TSubclassOf<UTGOR_Action>> Triggers;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set input value to currently running action (or triggers an action if part of trigger). */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SetInput(TSubclassOf<UTGOR_Input> Input, float Value);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
	   	
private:

	/** Input trigger values. */
	UPROPERTY()
		TMap<UTGOR_Input*, FTGOR_ActionTrigger> Inputs;

};
