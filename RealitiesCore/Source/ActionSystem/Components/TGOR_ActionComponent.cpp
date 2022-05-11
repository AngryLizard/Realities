// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "AnimationSystem/Components/TGOR_AnimationComponent.h"
#include "AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Content/TGOR_Performance.h"
#include "ActionSystem/Tasks/TGOR_ActionTask.h"
#include "ActionSystem/Content/TGOR_Action.h"
#include "ActionSystem/Content/TGOR_Input.h"
#include "ActionSystem/Content/Events/TGOR_Event.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "GameFramework/PlayerState.h"
#include "Templates/SubclassOf.h"
#include "Net/UnrealNetwork.h"
#include "Engine/NetConnection.h"
#include "Engine/ActorChannel.h"



FTGOR_ActionTrigger::FTGOR_ActionTrigger()
	: Value(0.0f), Trigger()
{
}

FTGOR_ActionTrigger::FTGOR_ActionTrigger(float Value, TSubclassOf<UTGOR_Action> Trigger)
	: Value(Value), Trigger(Trigger)
{
}

FTGOR_ActionSlotSpot::FTGOR_ActionSlotSpot()
{
}

UTGOR_ActionComponent::UTGOR_ActionComponent()
	: Super(),
	Verbose(false),
	PerformanceType(UTGOR_Performance::StaticClass())
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_ActionComponent::DestroyComponent(bool bPromoteChildren)
{
	for (UTGOR_ActionTask* ActionSlot : ActionSlots)
	{
		if (ActionSlot->IsRunning())
		{
			ActionSlot->Finish(ActionState);
		}
	}
	ActionSlots.Empty();

	Super::DestroyComponent(bPromoteChildren);
}

void UTGOR_ActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Update currently scheduled action
	if (ActionSlots.IsValidIndex(ActionState.ActiveSlot))
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[ActionState.ActiveSlot];
		if (IsValid(ActionSlot))
		{
			// Update action
			const float Timestep = UpdateActionTimestamp();
			if (Timestep >= SMALL_NUMBER)
			{
				// Make sure context is up to date with provided aim
				ActionSlot->Context(ActionState.Aim);

				ETGOR_ActionStateEnumeration OldState = ActionState.State;
				if (ActionSlot->Update(ActionState, Timestep))
				{
					// Replicate state immediately if changed
					if (OldState != ActionState.State)
					{
						const ENetRole NetRole = GetOwnerRole();
						if (NetRole == ENetRole::ROLE_AutonomousProxy)
						{
							SendState(ActionState);
						}
						else if (NetRole == ENetRole::ROLE_Authority)
						{
							// Make sure clients are updated asap
							ForceReplication();
						}
					}
				}
			}
		}
	}
}

void UTGOR_ActionComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ActionComponent, ActionSlots, COND_None); // This MUST be replicated before setup
	DOREPLIFETIME_CONDITION(UTGOR_ActionComponent, ActionSetup, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_ActionComponent, ActionState, COND_None);
}

void UTGOR_ActionComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	Super::GetSubobjectsWithStableNamesForNetworking(Objs);

	/*
	for (UTGOR_ActionTask* ActionSlot : ActionSlots)
	{
		if (IsValid(ActionSlot) && ActionSlot->IsNameStableForNetworking())
		{
			Objs.Add(ActionSlot);
		}
	}
	*/
}

bool UTGOR_ActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UTGOR_ActionTask* ActionSlot : ActionSlots)
	{
		if (IsValid(ActionSlot))
		{
			WroteSomething |= Channel->ReplicateSubobject(ActionSlot, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

TSet<UTGOR_CoreContent*> UTGOR_ActionComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> ActiveContent;
	const int32 Identifier = GetCurrentActionSlotIdentifier();

	// Currently active action
	UTGOR_Action* Action = GetAction(Identifier);
	if (IsValid(Action))
	{
		ActiveContent.Emplace(Action);
	}

	return ActiveContent;
}

void UTGOR_ActionComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	Dependencies.Process<UTGOR_PilotComponent>();
	Dependencies.Process<UTGOR_AnimationComponent>();
	Dependencies.Process<UTGOR_MovementComponent>();

	StaticActions = Dependencies.Spawner->GetMListFromType<UTGOR_Action>(SpawnStaticActions);
	SlotActions = Dependencies.Spawner->GetMListFromType<UTGOR_Action>(SpawnSlotActions);

	ApplyActionSetup(ActionSetup);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_ActionComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = ActionSlots.Num();
	for (UTGOR_ActionTask* ActionSlot : ActionSlots)
	{
		Modules.Emplace(ActionSlot->Identifier.Slot, ActionSlot->Identifier.Content);
	}
	return Modules;
}

TSubclassOf<UTGOR_Performance> UTGOR_ActionComponent::GetPerformanceType() const
{
	return PerformanceType;
}

UTGOR_AnimationComponent* UTGOR_ActionComponent::GetAnimationComponent() const
{
	return GetOwnerComponent<UTGOR_AnimationComponent>();
}

TArray<UTGOR_Modifier*> UTGOR_ActionComponent::QueryActiveModifiers_Implementation() const
{
	TArray<UTGOR_Modifier*> Modifiers;
	const int32 Identifier = GetCurrentActionSlotIdentifier();
	UTGOR_Action* Action = GetAction(Identifier);
	if (IsValid(Action))
	{
		Modifiers.Emplace(Action);
	}
	return Modifiers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionComponent::ForceReplication()
{
	AActor* OwningActor = GetOwner();
	if (OwningActor)
	{
		OwningActor->ForceNetUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_ActionComponent::ScheduleActionType(TSubclassOf<UTGOR_Action> Type)
{
	SINGLETON_RETCHK(false);

	float Last = FLT_MAX;
	int32 Identifier = INDEX_NONE;

	// Look for fitting action type
	for (int32 Slot = 0; Slot < ActionSlots.Num(); Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			UTGOR_Action* Action = ActionSlot->GetAction();
			if (IsValid(Action) && Action->IsA(Type))
			{
				const float Duration = ActionSlot->GetTimeSinceLastCall();
				if (ActionSlot->CanCall() && (Identifier == INDEX_NONE || Duration < Last))
				{
					Identifier = Slot;
					Last = Duration;
				}
			}
		}
	}

	return ScheduleSlotAction(Identifier);
}

bool UTGOR_ActionComponent::IsRunningActionType(TSubclassOf<UTGOR_Action> Type) const
{
	if (*Type && IsRunningAny())
	{
		UTGOR_Action* Action = GetAction(ActionState.ActiveSlot);
		if (IsValid(Action) && Action->IsA(Type))
		{
			return true;
		}
	}
	return false;
}

bool UTGOR_ActionComponent::IsOperatingActionType(TSubclassOf<UTGOR_Action> Type) const
{
	if (*Type && IsRunningAny())
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[ActionState.ActiveSlot];
		return IsValid(ActionSlot) && ActionSlot->GetAction()->IsA(Type) && ActionState.State == ETGOR_ActionStateEnumeration::Operate;
	}
	return false;
}

bool UTGOR_ActionComponent::IsRunningActionSupporting(const UTGOR_Action* Action) const
{
	if (IsRunningAny())
	{
		return GetAction(ActionState.ActiveSlot)->Instanced_ChildInsertions.Contains(Action);
	}
	return false;
}

int32 UTGOR_ActionComponent::GetCurrentActionSlotIdentifier() const
{
	return ActionState.ActiveSlot;
}

bool UTGOR_ActionComponent::ScheduleSlotAction(int32 Identifier)
{
	if (RunSlotAction(Identifier, false))
	{
		if (IsServer())
		{
			// Make sure clients are updated asap
			ForceReplication();
		}
		else if (IsLocallyControlled())
		{
			// Send to server if we're in control
			SendState(ActionState);
		}

		if (IsRunningAt(Identifier))
		{
			OnActionScheduled.Broadcast(ActionSlots[Identifier]);
		}
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActionComponent::IsRunning(UTGOR_Action* Action) const
{
	if (IsRunningAny())
	{
		return GetAction(ActionState.ActiveSlot) == Action;
	}
	return false;
}

bool UTGOR_ActionComponent::IsRunningAt(int32 Identifier) const
{
	return IsRunningAny() && Identifier == ActionState.ActiveSlot;
}

bool UTGOR_ActionComponent::IsRunningAny() const
{
	return ActionSlots.IsValidIndex(ActionState.ActiveSlot) && IsValid(ActionSlots[ActionState.ActiveSlot]->GetAction());
}

bool UTGOR_ActionComponent::HasProgress() const
{
	if (IsRunningAt(ActionState.ActiveSlot))
	{
		return ActionState.State == ETGOR_ActionStateEnumeration::Prepare || ActionState.State == ETGOR_ActionStateEnumeration::Finish;
	}
	return false;
}

float UTGOR_ActionComponent::GetProgress() const
{
	SINGLETON_RETCHK(0.0f);
	if (IsRunningAt(ActionState.ActiveSlot))
	{
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
		const float Duration = Timestamp - ActionState.Time;

		UTGOR_Action* Action = GetAction(ActionState.ActiveSlot);
		if (IsValid(Action))
		{
			if (ActionState.State == ETGOR_ActionStateEnumeration::Prepare)
			{
				return FMath::Clamp(Duration / Action->MaxPrepareTime, 0.0f, 1.0f);
			}
			if (ActionState.State == ETGOR_ActionStateEnumeration::Finish)
			{
				return FMath::Clamp(Duration / Action->MaxFinishTime, 0.0f, 1.0f);
			}
		}
	}
	return 0.0f;
}

UTGOR_Action* UTGOR_ActionComponent::GetAction(int32 Identifier) const
{
	if (ActionSlots.IsValidIndex(Identifier))
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Identifier];
		if (IsValid(ActionSlot))
		{
			return ActionSlot->GetAction();
		}
	}
	return nullptr;
}

bool UTGOR_ActionComponent::CollectDebugInfo(int32 Identifier, float Duration, FTGOR_ActionDebugInfo& Info) const
{
	if (ActionSlots.IsValidIndex(Identifier))
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Identifier];
		if (IsValid(ActionSlot))
		{
			return ActionSlot->CollectDebugInfo(Duration, Info);
		}
	}
	return false;
}

void UTGOR_ActionComponent::CollectDebugInfos(float Duration, TArray<FTGOR_ActionDebugInfo>& Infos) const
{
	const int32 SlotNum = ActionSlots.Num();
	for (int32 Slot = 0; Slot < SlotNum; Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			FTGOR_ActionDebugInfo Info;
			if (ActionSlot->CollectDebugInfo(Duration, Info))
			{
				Infos.Emplace(Info);
			}
		}
	}
}

TArray<int32> UTGOR_ActionComponent::GetCallableActionIdentifiers(TSubclassOf<UTGOR_Action> Type, bool CheckCanCall)
{
	TArray<int32> Identifiers;
	if (!*Type) return Identifiers;

	for (int32 Slot = 0; Slot < ActionSlots.Num(); Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			UTGOR_Action* Action = ActionSlot->GetAction();
			if (IsValid(Action) && Action->IsA(Type) && (!CheckCanCall || ActionSlot->CanCall()))
			{
				Identifiers.Emplace(Slot);
			}
		}
	}
	return Identifiers;
}

TArray<int32> UTGOR_ActionComponent::GetCallableSubactionIdentifiers(UTGOR_Action* Action, bool CheckCanCall)
{
	TArray<int32> Identifiers;
	if (!IsValid(Action)) return Identifiers;

	for (int32 Slot = 0; Slot < ActionSlots.Num(); Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			if (Action->Instanced_ChildInsertions.Contains(ActionSlot->GetAction()) && (!CheckCanCall || ActionSlot->CanCall()))
			{
				Identifiers.Emplace(Slot);
			}
		}
	}
	return Identifiers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_Time UTGOR_ActionComponent::GetServerTime(float Delay)
{
	SINGLETON_RETCHK(FTGOR_Time());

	return(Singleton->GetGameTimestamp() + Delay);
}


float UTGOR_ActionComponent::GetClientPing()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn))
	{
		return 0.0f;
	}

	APlayerState* PlayerState = Pawn->GetPlayerState();
	if (!IsValid(PlayerState))
	{
		return 0.0f;
	}

	return PlayerState->GetPingInMilliseconds();
}

void UTGOR_ActionComponent::GetNetOccupation(ETGOR_NetOccupation& Branches)
{
	Branches = ETGOR_NetOccupation::Invalid;
	if (IsLocallyControlled())
	{
		if (IsServer())
		{
			Branches = ETGOR_NetOccupation::Host;
		}
		else
		{
			Branches = ETGOR_NetOccupation::Client;
		}
	}
	else
	{
		if (IsServer())
		{
			Branches = ETGOR_NetOccupation::Client;
		}
		else
		{
			Branches = ETGOR_NetOccupation::Remote;
		}
	}
}

bool UTGOR_ActionComponent::IsLocallyControlled() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (IsValid(Pawn))
	{
		return Pawn->IsLocallyControlled();
	}
	return false;
}

bool UTGOR_ActionComponent::IsServer() const
{
	AActor* Actor = GetOwner();
	if (!IsValid(Actor))
	{
		return false;
	}

	return Actor->HasAuthority();
}

void UTGOR_ActionComponent::LocalBranch(ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;
	if (IsLocallyControlled())
	{
		Branches = ETGOR_BoolEnumeration::Is;
	}
}

void UTGOR_ActionComponent::ServerBranch(ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;
	if (IsServer())
	{
		Branches = ETGOR_BoolEnumeration::Is;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActionComponent::CanCallEvent(UTGOR_Event* Event) const
{
	if (IsRunningAny() && IsValid(Event) && (ActionState.State == ETGOR_ActionStateEnumeration::Operate || !Event->OperateOnly) && (IsServer() || !Event->AuthorityOnly))
	{
		UTGOR_Action* Action = GetAction(ActionState.ActiveSlot);
		return Action->Instanced_EventInsertions.Contains(Event);
	}
	return false;
}

void UTGOR_ActionComponent::CallEvent(UTGOR_Event* Event, const FTGOR_EventInstance& Params)
{
	if (CanCallEvent(Event))
	{
		if (Event->EventPriority == ETGOR_NetvarEnumeration::Local)
		{
			// Calling Implementation is not replicated
			CallEventClient_Implementation(Event, Params);
		}
		else if (IsServer())
		{
			if (Event->EventPriority >= ETGOR_NetvarEnumeration::Server)
			{
				CallEventClient(Event, Params);
			}
		}
		else
		{
			if (Event->EventPriority >= ETGOR_NetvarEnumeration::Client)
			{
				CallEventServer(Event, Params);
			}
		}
	}
}

void UTGOR_ActionComponent::CallEventServer_Implementation(UTGOR_Event* Event, FTGOR_EventInstance Params)
{
	if (CanCallEvent(Event))
	{
		// Also calls the event on server
		CallEventClient(Event, Params);
	}
}

bool UTGOR_ActionComponent::CallEventServer_Validate(UTGOR_Event* Event, FTGOR_EventInstance Params)
{
	return true;
}

void UTGOR_ActionComponent::CallEventClient_Implementation(UTGOR_Event* Event, FTGOR_EventInstance Params)
{
	if (CanCallEvent(Event))
	{
		Event->Call(ActionSlots[ActionState.ActiveSlot], Params);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ActionComponent::RepNotifyActionState(const FTGOR_ActionState& Old)
{
	const FTGOR_ActionState TargetState = ActionState;
	ActionState = Old;

	FastForwardState(TargetState);
}

void UTGOR_ActionComponent::FastForwardState(const FTGOR_ActionState& TargetState)
{
	SINGLETON_CHK;
	const bool IsSameAction = (ActionState.ActiveSlot == TargetState.ActiveSlot);

	UTGOR_ActionTask* SourceActionSlot = nullptr;
	if (ActionSlots.IsValidIndex(ActionState.ActiveSlot))
	{
		SourceActionSlot = ActionSlots[ActionState.ActiveSlot];
	}

	// Update action if we have a valid target
	if (ActionSlots.IsValidIndex(TargetState.ActiveSlot))
	{
		UTGOR_ActionTask* TargetActionSlot = ActionSlots[TargetState.ActiveSlot];

		// Handle currently running action in case we want to kill it
		if (IsValid(SourceActionSlot))
		{

			// Process kill command only if we're still running
			if (TargetState.State == ETGOR_ActionStateEnumeration::Dead)
			{
				if (IsSameAction && SourceActionSlot->IsRunningIn(ActionState))
				{
					SourceActionSlot->LogActionMessage("FastForward", "Interrupt from dead target");
					SourceActionSlot->Finish(ActionState);
				}
				// Assumably this action was already killed by the client themselves.
				// TODO: Technically this *could* be bad in case the action on the server did something different and important in its dying breath.
				//		 We can't just call it since action parameters might be out of date.
				return;
			}

			// Interrupt old action if we're replacing with a new one
			if (!IsSameAction)
			{
				SourceActionSlot->LogActionMessage("FastForward", FString("Interrupt from different action: ") + TargetActionSlot->GetName());
				SourceActionSlot->Finish(ActionState);
			}
		}

		// No need to go further if action is already dead
		if (TargetState.State == ETGOR_ActionStateEnumeration::Dead)
		{
			return;
		}

		// If updated action is new, schedule it properly
		if (!IsSameAction)
		{
			// Need to schedule so forwarding works
			RunSlotAction(TargetState.ActiveSlot, true);
		}

		// Forward action state
		if (TargetActionSlot->Forward(ActionState, TargetState.State))
		{
			check(ActionState.ActiveSlot == TargetState.ActiveSlot && "action got terminated but Forwarding still returned true");

			// Update action immediately in case multiple replications come in
			/* TODO: Not necessary, Forward already calls all necessary functions 
			const float Timestep = UpdateActionTimestamp();
			TargetActionSlot->Update(ActionState, Timestep);
			*/
		}
	}
	else if (IsValid(SourceActionSlot))
	{
		SourceActionSlot->LogActionMessage("FastForward", "Interrupt from invalidation");
		SourceActionSlot->Finish(ActionState);
	}
	else
	{
		ActionState.ActiveSlot = INDEX_NONE;
		ActionState.Time = Singleton->GetGameTimestamp();
		ActionState.State = ETGOR_ActionStateEnumeration::Suspended;
	}
}

bool UTGOR_ActionComponent::RunSlotAction(int32 Identifier, bool Force)
{
	SINGLETON_RETCHK(false);

	if (ActionSlots.IsValidIndex(Identifier))
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Identifier];
		if (IsValid(ActionSlot))
		{
			// Check whether this action can be called at all
			if (Force || ActionSlot->CanCall())
			{
				// Make sure to terminate previous action
				if (ActionSlots.IsValidIndex(ActionState.ActiveSlot))
				{
					UTGOR_ActionTask* OtherSlot = ActionSlots[ActionState.ActiveSlot];
					if (IsValid(OtherSlot))
					{
						if (ActionState.ActiveSlot == Identifier)
						{
							// No need to run what's already running
							OtherSlot->LogActionMessage("RunSlotAction", "Tried to rerun same action");
							return false;
						}

						OtherSlot->LogActionMessage("RunSlotAction", FString("Interrupted from override: ") + OtherSlot->GetName());
						OtherSlot->Finish(ActionState);
					}
				}

				// Actually schedule action
				ActionSlot->Prepare(ActionState);

				// Make sure context is up to date
				ActionSlot->Context(ActionState.Aim);
				return true;
			}
			else if(!Force)
			{
				ActionSlot->LogActionMessage("RunSlotAction", "CanCall prevented calling");
			}
		}
	}
	else if (ActionSlots.IsValidIndex(ActionState.ActiveSlot))
	{
		UTGOR_ActionTask* OtherSlot = ActionSlots[ActionState.ActiveSlot];
		if (IsValid(OtherSlot))
		{
			OtherSlot->LogActionMessage("RunSlotAction", FString("Interrupted from empty override"));
			OtherSlot->Finish(ActionState);
		}
	}
	return false;
}


void UTGOR_ActionComponent::SendState_Implementation(FTGOR_ActionState State)
{
	if (ActionSlots.IsValidIndex(State.ActiveSlot))
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[State.ActiveSlot];
		if (IsValid(ActionSlot))
		{
			// Schedule new action if we're not running it yet
			if (!ActionSlot->IsRunning())
			{
				// We only take properties from client if action isn't already running
				ActionSlot->Context(State.Aim);

				// Schedule action on server
				if (!ScheduleSlotAction(State.ActiveSlot))
				{
					// Owned client apparently is getting wrong ideas, scold them back into place
					// (This way replication is forced whether or not new action is scheduled)
					ForceReplication();
					return;
				}
			}
			else if (ActionState.Aim.Component != State.Aim.Component || ActionState.Aim.Target != State.Aim.Target)
			{
				// Also update if client significantly disagrees with server about our current target
				ForceReplication();
			}

			// Update currently running action with state
			ActionSlot->Integrate(ActionState, State);
			ActionSlot->LogActionMessage("RunSlotAction", "Update state on server");
		}
	}
	else if (IsRunningAny())
	{
		ScheduleSlotAction(INDEX_NONE);
	}
	
}

bool UTGOR_ActionComponent::SendState_Validate(FTGOR_ActionState State)
{
	return true;
}



float UTGOR_ActionComponent::UpdateActionTimestamp()
{
	// Update cooldown
	if (ActionSlots.IsValidIndex(ActionState.ActiveSlot))
	{
		FTGOR_Time& LastCallTimestamp = ActionSlots[ActionState.ActiveSlot]->LastCallTimestamp;

		const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
		const float Timestep = GameTimestamp - LastCallTimestamp;
		LastCallTimestamp = GameTimestamp;
		return Timestep;
	}
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionComponent::SetInput(TSubclassOf<UTGOR_Input> InputType, float Value)
{
	bool HasHandledAction = false;

	if (ActionSlots.IsValidIndex(ActionState.ActiveSlot))
	{

	}

	const int32 SlotNum = ActionSlots.Num();
	for (int32 Slot = 0; Slot < SlotNum; Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			if (ActionSlot->UpdateInput(InputType, Value, !HasHandledAction))
			{
				// Only trigger one new action
				HasHandledAction = true;
			}
		}
	}

}

void UTGOR_ActionComponent::UpdateContext(const FTGOR_AimInstance& Aim)
{
	// TODO: Could do something fancy here like filter by target content
	// (though that would break range constraints and there is seemingly no gain to doing that currently)
	const int32 SlotNum = ActionSlots.Num();
	for (int32 Slot = 0; Slot < SlotNum; Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			ActionSlot->Context(Aim);

			/* Make exception for weapons where target changes continuously.
			* Need to replicate this to server efficiently. For now actions can just use the current aim component for targets that don't need a handshake.
			if (Slot == ActionState.ActiveSlot)
			{
				ActionState.Aim = Aim;
			}
			*/
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActionComponent::ApplyActionSetup(FTGOR_LoadoutInstance Setup)
{
	TMap<UTGOR_Action*, TArray<UTGOR_ActionTask*>> Previous;

	// Remove slots but cache both instances and items in case the new loadout can use them
	for (int Slot = 0; Slot < ActionSlots.Num(); Slot++)
	{
		UTGOR_ActionTask* ActionSlot = ActionSlots[Slot];
		if (IsValid(ActionSlot))
		{
			Previous.FindOrAdd(ActionSlot->GetAction()).Add(ActionSlot);
		}
	}

	ActionSlots.Empty();
	ActionSetup.Actions.Empty();

	// Add static slots
	TArray<UTGOR_Action*> Actions = StaticActions;

	// Add optional slots
	const int32 SlotNum = ActionSlotSpots.Num();
	for (int32 Slot = 0; Slot < SlotNum; Slot++)
	{
		const FTGOR_ActionSlotSpot& ActionSlotSpot = ActionSlotSpots[Slot];

		UTGOR_Action* Action = nullptr;

		// Set slot to setup
		if (Setup.Actions.IsValidIndex(Slot))
		{
			Action = Setup.Actions[Slot];
			if (!IsValid(Action) || !Action->IsA(ActionSlotSpot.Type) || !SlotActions.Contains(Action))
			{
				Action = nullptr;
			}
		}

		// Get first valid action from slot list if obligatory
		if (!IsValid(Action) && *ActionSlotSpot.Obligatory)
		{
			for (UTGOR_Action* SlotAction : SlotActions)
			{
				if (SlotAction->IsA(ActionSlotSpot.Obligatory))
				{
					Action = SlotAction;
					continue;
				}
			}
		}

		if (IsValid(Action))
		{
			Actions.Emplace(Action);
		}

		// Rebuild current setup for clients
		ActionSetup.Actions.Emplace(Action);
	}

	// Generate slots
	for (UTGOR_Action* Action : Actions)
	{
		if (IsValid(Action))
		{
			UTGOR_ActionTask* ActionSlot = nullptr;

			TArray<UTGOR_ActionTask*>* Ptr = Previous.Find(Action);
			if (Ptr && Ptr->Num() > 0)
			{
				ActionSlot = Ptr->Pop();
			}
			else
			{
				// No cache was found, create a new one
				ActionSlot = Action->CreateActionTask(this, ActionSlots.Num());
			}

			if (IsValid(ActionSlot))
			{
				ActionSlots.Add(ActionSlot);
			}
		}
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_ActionTask* ActionSlot : Pair.Value)
		{
			ActionSlot->MarkAsGarbage();
		}
	}

	OnActionUpdated.Broadcast();
	return true;
}

void UTGOR_ActionComponent::OnReplicateActionSetup()
{
	ApplyActionSetup(ActionSetup);
}
