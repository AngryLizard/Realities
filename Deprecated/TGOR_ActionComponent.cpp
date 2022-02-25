// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionComponent.h"

#include "Mod/Actions/TGOR_Action.h"
#include "GameFramework/PlayerState.h"
#include "Base/Controller/TGOR_PlayerController.h"
#include "Base/TGOR_Singleton.h"

#define ACTION_VERBOSE(T, N) if (Verbose) {EOUT(FString::FromInt(N.Index) + ":" + FString(T))}
#define CONTENT_VERBOSE(T, N) if (Verbose) {EOUT(FString::FromInt(N) + ":" + FString(T))}


UTGOR_ActionComponent::UTGOR_ActionComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;

	Verbose = false;
	InactiveTimeout = 2.0f;
}


void UTGOR_ActionComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTGOR_ActionComponent, TriggerQueue);
}

void UTGOR_ActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SINGLETON_CHK
	const float Timestamp = Singleton->GameTimestamp;

	// Update all triggers
	const int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num-1; i >= 0; i--)
	{
		FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (IsValid(Trigger.Action))
		{
			// Ensure component as owner
			Trigger.Owner = this;

			// Prepare action if newly spawned
			if (Trigger.Time < 0.0f)
			{
				// Invalid action signifies newly spawned action
				Trigger.Action->Prepare(this, Trigger, Timestamp);

				//ACTION_VERBOSE(Trigger.Storage.Print(), Trigger.Identifier)
				ACTION_VERBOSE("Prepared action", Trigger.Identifier)
			}

			// Add or replace action cooldown
			const float Cooldown = Trigger.Action->ScheduleCooldown;
			if (Cooldown > SMALL_NUMBER)
			{
				const int32 ActionIndex = Trigger.Action->DefaultIndex;
				CooldownQueue.Add(ActionIndex, Timestamp + Cooldown);
			}

			// Update Action
			Update(Trigger, i, Timestamp);
		}
		else
		{
			ACTION_VERBOSE("Invalid action", Trigger.Identifier)
		}
	}

	// Maintain cooldown
	for (auto Pair = CooldownQueue.CreateIterator(); Pair; ++Pair)
	{
		// Remove cooldown if timer reached
		float& Timer = Pair->Value;
		if (Timestamp > Timer)
		{
			Pair.RemoveCurrent();
			CONTENT_VERBOSE("Cooldown expired", Pair->Key)
		}
	}
}


void UTGOR_ActionComponent::BeginDestroy()
{
	// Interrupt all triggers
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (IsValid(Trigger.Action))
		{
			Trigger.Action->Interrupt(this, Trigger);
			ACTION_VERBOSE("Interrupted action", Trigger.Identifier)
		}
		else
		{
			ACTION_VERBOSE("Invalid action", Trigger.Identifier)
		}
	}
	TriggerQueue.Trigger.Empty();

	Super::BeginDestroy();
}

void UTGOR_ActionComponent::Update(FTGOR_ActionTrigger& Trigger, int32 Index, float Timestamp)
{
	// Update Action if active
	if (Trigger.Active)
	{
		// Measure time since last call (make sure not to go back in time)
		float Delta = FMath::Max(0.0f,Timestamp - Trigger.Time);
		Trigger.Time = Timestamp;

		// Update
		bool Valid = Trigger.Action->Update(this, Trigger, Delta);

		// If any variables got changed, send/replicate them (Even if the action gets killed to make sure the server terminates also)
		if (Trigger.Storage.Resend)
		{
			// Send to server if client
			if (!IsServer())
			{
				// Make sure variables get send reliably if action gets destroyed next
				if (Trigger.Storage.Importance == ETGOR_ReplicationType::Reliable || !Valid)
				{
					SetVariablesReliable(Trigger.Identifier, Trigger.Storage);
				}
				else
				{
					SetVariablesUnreliable(Trigger.Identifier, Trigger.Storage);
				}
			}

			// Mark dirty
			TriggerQueue.MarkItemDirty(Trigger);
		}

		Trigger.Storage.SetResend(false);

		// Kill action if invalidated during this update
		if (!Valid)
		{
			ACTION_VERBOSE("Action invalidated from update", Trigger.Identifier)
				ActionKill(Index);
		}
	}
	else if(Timestamp > Trigger.Time + InactiveTimeout)
	{
		ACTION_VERBOSE("Inactive action forcefully killed.", Trigger.Identifier)
		Trigger.Action = nullptr;
		ActionKill(Index);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ActionComponent::GetServerTime(float Delay)
{
	SINGLETON_RETCHK(0.0f);
	
	return(Singleton->GameTimestamp + Delay);
}


float UTGOR_ActionComponent::GetClientPing()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn))
	{
		return(0.0f);
	}

	APlayerState* PlayerState = Pawn->GetPlayerState();
	if (!IsValid(PlayerState))
	{
		return(0.0f);
	}

	return(PlayerState->Ping);
}


FTGOR_ActionIndex UTGOR_ActionComponent::GenerateIdentifier()
{
	// Get unique number from counter and wrap around
	if (_counter == 0xFF)
	{
		_counter = 0;
	}
	// TODO: Check whether already exists

	// Avoid 0
	return(FTGOR_ActionIndex(++_counter));
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

bool UTGOR_ActionComponent::IsLocallyControlled()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (IsValid(Pawn))
	{
		return (Pawn->IsLocallyControlled());
	}
	return(false);
}

bool UTGOR_ActionComponent::IsServer()
{
	AActor* Actor = GetOwner();
	if (!IsValid(Actor))
	{
		return(false);
	}

	return (Actor->HasAuthority());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ActionComponent::GetActionIndexCooldown(int32 ActionIndex)
{
	float* Cooldown = CooldownQueue.Find(ActionIndex);
	if (Cooldown == nullptr)
	{
		return(0.0f);
	}

	SINGLETON_RETCHK(0.0f)
	const float Timestamp = Singleton->GameTimestamp;
	return((*Cooldown) - Timestamp);
}

float UTGOR_ActionComponent::GetActionCooldown(TSubclassOf<UTGOR_Action> Action)
{
	SINGLETON_RETCHK(false);

	int32 ActionIndex = Singleton->ContentManager->Find<UTGOR_Action>(Action);
	if (ActionIndex == -1)
	{
		ERRET("Action not found.", Error, false)
	}
	return(GetActionIndexCooldown(ActionIndex));
}

void UTGOR_ActionComponent::AddCooldown(const FTGOR_ActionTrigger& Trigger, float Cooldown)
{
	// Add cooldown to client
	ETGOR_NetOccupation Occupation;
	GetNetOccupation(Occupation);
	if (Occupation == ETGOR_NetOccupation::Host ||
		Occupation == ETGOR_NetOccupation::Client)
	{
		// Compute gametime when cooldown runs out
		const float Timestamp = GetServerTime(Cooldown);
		CooldownQueue.Add(Trigger.Action->DefaultIndex, Timestamp);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Action* UTGOR_ActionComponent::GetScheduledAction(FTGOR_ActionIndex Identifier)
{
	// Find matching action
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (Trigger.Identifier.matches(Identifier))
		{
			return(Trigger.Action);
		}
	}
	return(nullptr);
}

bool UTGOR_ActionComponent::IsActionScheduled(FTGOR_ActionIndex Identifier)
{
	return(IsValid(GetScheduledAction(Identifier)));
}

bool UTGOR_ActionComponent::IsAnyActionScheduled(UTGOR_Action* Action)
{
	// Find matching action
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (Trigger.Action == Action)
		{
			return(true);
		}
	}
	return(false);
}

bool UTGOR_ActionComponent::IsAnyActionClassScheduled(TSubclassOf<UTGOR_Action> ActionClass)
{
	// Find matching action
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (Trigger.Action->IsA(ActionClass))
		{
			return(true);
		}
	}
	return(false);
}




FTGOR_ActionIndex UTGOR_ActionComponent::ScheduleParamsAction(TSubclassOf<UTGOR_Action> ActionClass, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	SINGLETON_RETCHK(0);
	
	// Get action resourceable
	UTGOR_Action* Action = Singleton->ContentManager->GetByKey(ActionClass);
	if (!IsValid(Action))
	{
		ERRET("Action not found.", Error, -1)
	}

	Branches = ETGOR_ComputeEnumeration::Success;
	return(ScheduleAction(Action, Params, Branches));
}

FTGOR_ActionIndex UTGOR_ActionComponent::ScheduleActionIndex(int32 ActionIndex, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	SINGLETON_RETCHK(0);
	
	// Get action resourceable
	UTGOR_Action* Action = Singleton->ContentManager->GetByIndex<UTGOR_Action>(ActionIndex);
	if (!IsValid(Action))
	{
		ERRET("Action not found.", Error, -1)
	}

	Branches = ETGOR_ComputeEnumeration::Success;
	return(ScheduleAction(Action, Params, Branches));
}

FTGOR_ActionIndex UTGOR_ActionComponent::ScheduleAction(UTGOR_Action* Action, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	// Ensure Action is defined
	if (!IsValid(Action))
	{
		ERRET("Action doesn't exist.", Error, -1)
	}
	
	// Test on client first if action can be called
	if (!Action->CanCall(this, Params))
	{
		ACTION_VERBOSE("CanCall false on schedule", FTGOR_ActionIndex())
		return(-1);
	}

	// Create trigger
	FTGOR_ActionTrigger Trigger;
	Trigger.Action = Action;
	Trigger.Storage = Params;
	Trigger.Storage.SetResend(true);

	// Initialise action parameters
	Trigger.Identifier = GenerateIdentifier();

	ACTION_VERBOSE("Register Action", Trigger.Identifier)

	// Precall and create the entry
	if (GetOwner()->HasAuthority())
	{
		// Register on server
		RegisterAction(Trigger);
	}
	else if (!Action->AuthorityOnly)
	{
		// Trigger and request on server
		ACTION_VERBOSE("Add trigger immediately as client", Trigger.Identifier)
		TriggerAction(Trigger, GetServerTime(0.0f));
		RequestRegisterAction(Trigger);
	}
	else
	{
		ACTION_VERBOSE("Tried to schedule Action on client that is authority only", Trigger.Identifier)
	}


	Branches = ETGOR_ComputeEnumeration::Success;
	return(Trigger.Identifier);
}

void UTGOR_ActionComponent::RequestRegisterAction_Implementation(FTGOR_ActionTrigger Trigger)
{
	SINGLETON_CHK

	// Ensure action resourceable
	if (IsValid(Trigger.Action))
	{
		if (!Trigger.Action->AuthorityOnly) 
		{
			RegisterAction(Trigger);
		}
		else 
		{
			ACTION_VERBOSE("Action requested that is authority only", Trigger.Identifier)
		}
	}
}

bool UTGOR_ActionComponent::RequestRegisterAction_Validate(FTGOR_ActionTrigger Trigger)
{
	return(true);
}

void UTGOR_ActionComponent::RegisterAction(FTGOR_ActionTrigger Trigger)
{
	SINGLETON_CHK

	// Ensure action resourceable
	if (IsValid(Trigger.Action))
	{
		// Lag compensation (max compesated ping)
		float Ping = FMath::Min(GetClientPing(), MAX_LAG_COMPENSATION);
		float Timestamp = GetServerTime(-Ping / 2);

		// Test on server if action can be called
		if (Trigger.Action->CanCall(this, Trigger.Storage))
		{
			ACTION_VERBOSE("Add trigger as server", Trigger.Identifier)
			TriggerAction(Trigger, Timestamp);
			GetOwner()->ForceNetUpdate();
		}
		else
		{
			ACTION_VERBOSE("CanCall false on server, interrupt all", Trigger.Identifier)
			// Deschedule on clients (interrupt)
			ActionKillNotify(Trigger.Identifier);
		}
	}
}


FTGOR_ActionTrigger& UTGOR_ActionComponent::TriggerAction(FTGOR_ActionTrigger& Trigger, float Time)
{
	// Create trigger
	Trigger.Active = true;

	// Add trigger to queue
	int32 Index = TriggerQueue.Trigger.Add(Trigger);
	FTGOR_ActionTrigger& NewTrigger = TriggerQueue.Trigger[Index];
	TriggerQueue.MarkItemDirty(NewTrigger);

	NewTrigger.Action->Prepare(this, NewTrigger, Time);

	ACTION_VERBOSE("Triggered action", NewTrigger.Identifier)
	ACTION_VERBOSE(NewTrigger.Storage.Print(), NewTrigger.Identifier)

	return(NewTrigger);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionComponent::ActionKillAll(TSubclassOf<UTGOR_Action> Action)
{
	// Make sure only called on server
	if (!IsServer())
	{
		ERROR("Can only kill actions on server.", Error)
		return;
	}

	SINGLETON_CHK;

	// Get action index
	int32 ActionIndex = Singleton->ContentManager->Find(Action);
	if (ActionIndex == -1)
	{
		ERROR("Action not found.", Error)
	}

	// Find matching action
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (Trigger.Action->IsA(Action))
		{
			ActionKillNotify(Trigger.Identifier);
		}
	}
}

void UTGOR_ActionComponent::ActionKillNotify_Implementation(FTGOR_ActionIndex Identifier)
{
	ACTION_VERBOSE("Killing action", Identifier)

	// Look for action to remove
	int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		const FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (Trigger.Identifier.matches(Identifier))
		{
			// Interrupt
			if (IsValid(Trigger.Action))
			{
				ACTION_VERBOSE("Interrupt action", Identifier)
				Trigger.Action->Interrupt(this, Trigger);
			}

			// No need to mark as dirty as this is already replicated to everyone
			TriggerQueue.Trigger.RemoveAt(i);
			TriggerQueue.MarkArrayDirty();
			ACTION_VERBOSE("Removed action", Identifier)
		}
	}
}

void UTGOR_ActionComponent::ActionKill(int32 Index)
{
	FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[Index];
	if (IsValid(Trigger.Action))
	{
		// Only remove on server and replicate to clients
		if (IsServer())
		{
			Trigger.Action->Interrupt(this, Trigger);
			TriggerQueue.Trigger.RemoveAt(Index);
			TriggerQueue.MarkArrayDirty();
			ACTION_VERBOSE("Server action killed", Trigger.Identifier)
		}
		else
		{
			// Set inactive on client to wait for deletion
			Trigger.Active = false;
			ACTION_VERBOSE("Set inactive on client", Trigger.Identifier)
		}
	}
	else
	{
		TriggerQueue.Trigger.RemoveAt(Index);
		TriggerQueue.MarkArrayDirty();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ActionComponent::SetVariablesReliable_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params)
{
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (Trigger.Identifier.matches(Identifier))
		{
			// Set storage
			Trigger.Storage.Merge(Trigger.Action, Params, ETGOR_VariableReplication::Autonomous);
			ACTION_VERBOSE("Set Reliable variable", Identifier)
			ACTION_VERBOSE(Params.Print(), Identifier)
		}
	}
}

bool UTGOR_ActionComponent::SetVariablesReliable_Validate(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params)
{
	return(true);
}


void UTGOR_ActionComponent::SetVariablesUnreliable_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params)
{
	for (FTGOR_ActionTrigger& Trigger : TriggerQueue.Trigger)
	{
		if (Trigger.Identifier.matches(Identifier))
		{
			// Set storage
			Trigger.Storage.Merge(Trigger.Action, Params, ETGOR_VariableReplication::Autonomous);
			ACTION_VERBOSE("Set Unreliable variable", Identifier)
			ACTION_VERBOSE(Params.Print(), Identifier)
		}
	}
}

bool UTGOR_ActionComponent::SetVariablesUnreliable_Validate(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params)
{
	return(true);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionComponent::ActionNotifyAll(TSubclassOf<UTGOR_Action> Action, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	SINGLETON_CHK;

	// Get action index
	int32 ActionIndex = Singleton->ContentManager->Find(Action);
	if (ActionIndex == -1)
	{
		ERROR("Action not found.", Error)
	}

	// Find matching action
	const int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (Trigger.Action->IsA(Action))
		{
			ActionNotifyTrigger(Trigger, i, Params, Branches);
		}
	}
}

void UTGOR_ActionComponent::ActionNotify(const FTGOR_ActionIndex& Identifier, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	// Find matching action
	const int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (Trigger.Identifier.matches(Identifier))
		{
			ActionNotifyTrigger(Trigger, i, Params, Branches);
			return;
		}
	}

	ACTION_VERBOSE("Identifier for notify not found", Identifier)
}

void UTGOR_ActionComponent::ActionNotifyTrigger(FTGOR_ActionTrigger& Trigger, int32 Index, const FTGOR_ActionStorage& Params, ETGOR_ComputeEnumeration& Branches)
{
	if (IsValid(Trigger.Action))
	{
		// Send all variables
		FTGOR_ActionStorage Storage = Params;
		Storage.SetResend(true);

		const float Timestamp = GetServerTime(0.0f);

		if (Trigger.Action->NotifyType == ETGOR_NotifyReplication::LocalOnly)
		{
			ACTION_VERBOSE("Notify locally only", Trigger.Identifier)

			// Call only locally
			if (Trigger.Action->Notify(this, Trigger, Storage, Timestamp))
			{
				Update(Trigger, Index, Timestamp);
			}
			else
			{
				ACTION_VERBOSE("Notify failed", Trigger.Identifier)
				Trigger.Action->Failed(this, Trigger, Storage, Timestamp);
			}
		}
		else if (Trigger.Action->NotifyType == ETGOR_NotifyReplication::Handshake || IsServer())
		{
			// Send immediately if doing handshake or calling from authority already
			ACTION_VERBOSE("Handshake/Server requested notify", Trigger.Identifier)
			RequestActionNotify(Trigger.Identifier, Storage);
		}
		else
		{
			ACTION_VERBOSE("Notify locally", Trigger.Identifier)

			// Only send if local notify succeeded
			if (Trigger.Action->Notify(this, Trigger, Storage, Timestamp))
			{
				RequestActionNotify(Trigger.Identifier, Storage);
				Update(Trigger, Index, Timestamp);
			}
			else
			{
				ACTION_VERBOSE("Notify failed", Trigger.Identifier)
				Trigger.Action->Failed(this, Trigger, Storage, Timestamp);
			}
		}

		Branches = ETGOR_ComputeEnumeration::Success;
		return;
	}
}

void UTGOR_ActionComponent::RequestActionNotify_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params)
{
	// Find matching action
	const int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (Trigger.Identifier.matches(Identifier))
		{
			if (IsValid(Trigger.Action))
			{
				ACTION_VERBOSE("Notify requested", Identifier)
				ACTION_VERBOSE(Params.Print(), Identifier)

				// Lag compensation (max compesated ping is 200ms)
				const float Ping = FMath::Min(GetClientPing(), MAX_LAG_COMPENSATION);
				const float Timestamp = GetServerTime(-Ping / 2);

				// Notify, check for failure
				if (Trigger.Action->Notify(this, Trigger, Params, Timestamp))
				{
					CommitActionNotify(Identifier, Params, Timestamp);
					Update(Trigger, i, Timestamp);
				}
				else
				{
					ACTION_VERBOSE("Notify request failed", Identifier)
					FailActionNotify(Identifier, Params, Timestamp);
				}

				return;
			}
		}
	}
}

bool UTGOR_ActionComponent::RequestActionNotify_Validate(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params)
{
	return(true);
}

void UTGOR_ActionComponent::CommitActionNotify_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params, float Time)
{
	// Ignore server (already called notify)
	ENetRole NetRole = GetOwnerRole();
	if (NetRole == ENetRole::ROLE_Authority)
	{
		return;
	}

	// Find matching action
	const int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (Trigger.Identifier.matches(Identifier))
		{
			ACTION_VERBOSE("Commit notify", Identifier)

			// Resurrect inactive actions
			if (!Trigger.Active)
			{
				Trigger.Active = true;
			}

			if (IsValid(Trigger.Action))
			{
				// Already called on autonomous proxy if not doing handshake
				if (Trigger.Action->NotifyType == ETGOR_NotifyReplication::Handshake || NetRole < ENetRole::ROLE_AutonomousProxy)
				{
					ACTION_VERBOSE("Commit handshaked notify", Identifier)
					if (Trigger.Action->Notify(this, Trigger, Params, Time))
					{
						Update(Trigger, i, Time);
					}
					return;
				}
			}
		}
	}
}

void UTGOR_ActionComponent::FailActionNotify_Implementation(FTGOR_ActionIndex Identifier, FTGOR_ActionStorage Params, float Time)
{
	// Find matching action
	const int32 Num = TriggerQueue.Trigger.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		FTGOR_ActionTrigger& Trigger = TriggerQueue.Trigger[i];
		if (Trigger.Identifier.matches(Identifier))
		{
			// Resurrect inactive actions
			if (!Trigger.Active)
			{
				Trigger.Active = true;
			}

			if (IsValid(Trigger.Action))
			{
				ACTION_VERBOSE("Action notify failed", Identifier)
				Trigger.Action->Failed(this, Trigger, Params, Time);
				Update(Trigger, i, Time);
			}
		}
	}
}