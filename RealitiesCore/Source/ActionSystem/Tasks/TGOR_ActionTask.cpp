// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionTask.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "../Content/TGOR_Input.h"
#include "../Content/Events/TGOR_Event.h"
#include "TargetSystem/Content/TGOR_Target.h"
#include "ActionSystem/Components/TGOR_ActionComponent.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "AnimationSystem/Components/TGOR_ArmatureComponent.h"
#include "MovementSystem/Tasks/TGOR_MovementTask.h"
#include "MovementSystem/Content/TGOR_Movement.h"
#include "ActionSystem/Content/TGOR_Action.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "AnimationSystem/Content/TGOR_Animation.h"

#include "Net/UnrealNetwork.h"


UTGOR_ActionTask::UTGOR_ActionTask()
	: Super()
{
}

void UTGOR_ActionTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ActionTask, Identifier, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UTGOR_ActionTask, LastCallTimestamp, COND_None);
}

TScriptInterface<ITGOR_AnimationInterface> UTGOR_ActionTask::GetAnimationOwner() const
{
	return GetOwnerComponent();
}

void UTGOR_ActionTask::LogActionMessage(const FString& Domain, const FString& Message)
{
	if(IsValid(Identifier.Content) && IsValid(Identifier.Component) && Singleton.IsValid() && Identifier.Content->Verbose)
	{
		FTGOR_ActionLog Log;
		Log.Domain = Domain;
		Log.Message = Message;
		Log.Timestamp = Singleton->GetGameTimestamp();

		DebugLogs.Emplace(Log);
		if (DebugLogs.Num() > 64)
		{
			DebugLogs.RemoveAt(0);
		}

		if (Identifier.Component->Verbose)
		{
			const FString Name = Identifier.Content->GetDefaultName();

			AActor* Owner = Identifier.Component->GetOwner();
			const FString OwnerName = Owner->GetName();
			const FString OnwerRole =
				Identifier.Component->GetOwnerRole() == ENetRole::ROLE_Authority ? "Server" :
				Identifier.Component->GetOwnerRole() == ENetRole::ROLE_AutonomousProxy ? "Client" : "Simulated";
			UE_LOG(TGOR_ActionLogs, Log, TEXT("%s / %s - %s: %s"), *OnwerRole, *OwnerName, *Name, *Message);
		}
		else
		{
			//UE_LOG(TGOR_ActionLogs, Log, TEXT("None - %s: %s"), *Name, *Message);
		}
	}
}

bool UTGOR_ActionTask::CanCall() const
{
	if (IsValid(Identifier.Component) && IsValid(Identifier.Content))
	{
		// Check invariant on empty params
		return Condition();
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionTask::Initialise()
{
	if (IsValid(Identifier.Component) && IsValid(Identifier.Content))
	{
		MovementComponent = Identifier.Component->GetOwnerComponent<UTGOR_MovementComponent>();
		PilotComponent = Identifier.Component->GetOwnerRootScene<UTGOR_ArmatureComponent>();
		
		Identifier.Content->TaskInitialise(this);
	}

	OnInitialise();
}

bool UTGOR_ActionTask::Condition() const
{
	if (CheckOwnerState())
	{
		ETGOR_ValidEnumeration Invariance = ETGOR_ValidEnumeration::Valid;
		OnCondition(Invariance);
		return Invariance == ETGOR_ValidEnumeration::Valid;
	}
	return false;
}

void UTGOR_ActionTask::PrepareStart()
{
	PlayAnimation();
	OnPrepareStart();
}

bool UTGOR_ActionTask::PrepareState(float Time, float Deltatime)
{
	ETGOR_StateEnumeration Wait = ETGOR_StateEnumeration::NextState;
	OnPrepare(Time, Deltatime, Wait);
	Animate(Deltatime);
	return Wait == ETGOR_StateEnumeration::KeepState;
}

void UTGOR_ActionTask::OperateStart()
{
	OnOperateStart();
}

bool UTGOR_ActionTask::OperateState(float Time, float Deltatime)
{
	ETGOR_StateEnumeration Wait = ETGOR_StateEnumeration::NextState;
	OnOperate(Time, Deltatime, Wait);
	Animate(Deltatime);
	return Wait == ETGOR_StateEnumeration::KeepState;
}

void UTGOR_ActionTask::FinishStart()
{
	OnFinishStart();
}

bool UTGOR_ActionTask::FinishState(float Time, float Deltatime)
{
	ETGOR_StateEnumeration Wait = ETGOR_StateEnumeration::NextState;
	OnFinish(Time, Deltatime, Wait);
	Animate(Deltatime);
	return Wait == ETGOR_StateEnumeration::KeepState;
}

void UTGOR_ActionTask::Interrupt()
{
	ResetAnimation();
	OnInterrupt();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Action* UTGOR_ActionTask::GetAction() const
{
	return Identifier.Content;
}

UTGOR_ActionComponent* UTGOR_ActionTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_ActionTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ActionTask::GetTimeSinceLastCall() const
{
	SINGLETON_RETCHK(0.0f);

	// Check cooldown
	return Singleton->GetGameTimestamp() - LastCallTimestamp;
}

float UTGOR_ActionTask::GetDuration(const FTGOR_ActionState& State) const
{
	SINGLETON_RETCHK(0.0f);
	return FMath::Max(0.0f, Singleton->GetGameTimestamp() - State.Time);
}

bool UTGOR_ActionTask::IsRunningIn(const FTGOR_ActionState& State) const
{
	return State.ActiveSlot == Identifier.Slot;
}

bool UTGOR_ActionTask::IsRunning() const
{
	if (IsValid(Identifier.Component))
	{
		return Identifier.Component->IsRunningAt(Identifier.Slot);
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActionTask::UpdateInput(TSubclassOf<UTGOR_Input> InputType, float Value, bool AllowTrigger)
{
	bool Handled = false;
	bool HasTrigger = false;

	// Update input values in slot
	for (const auto& InputPair : Identifier.Content->Instanced_InputInsertions.GetListOfType(InputType))
	{
		bool ReadyForTrigger = true;

		float* Ptr = Inputs.Inputs.Find(InputPair.Key);
		if (Ptr)
		{
			// Only do stuff if we actually changed
			if (!FMath::IsNearlyEqual((*Ptr), Value))
			{
				if (InputPair.Key->IsActive((*Ptr)))
				{
					ReadyForTrigger = false; // Input is already on
				}

				(*Ptr) = Value; // Update value

				if (IsRunning())
				{
					OnInputChanged(InputPair.Key, Value);
					ReadyForTrigger = false; // We're already running
					Handled = true;
				}
			}
		}
		// Never add inputs that we only use for action triggers
		else if (InputPair.Value != ETGOR_InputTrigger::TriggerOnly)
		{
			Inputs.Inputs.Emplace(InputPair.Key, Value);
		}

		// Look for inputs that schedule actions
		if (ReadyForTrigger && 
			InputPair.Value != ETGOR_InputTrigger::InputOnly && 
			InputPair.Key->IsActive(Value) && 
			!HasTrigger)
		{
			HasTrigger = true;
		}
	}

	if (HasTrigger && AllowTrigger)
	{
		Handled = Identifier.Component->ScheduleSlotAction(Identifier.Slot);
	}

	return Handled;
}

bool UTGOR_ActionTask::IsActiveInput(TSubclassOf<UTGOR_Input> InputType) const
{
	for (const auto& Pair : Inputs.Inputs)
	{
		if (IsValid(Pair.Key) && Pair.Key->IsA(InputType) && Pair.Key->IsActive(Pair.Value))
		{
			return true;
		}
	}
	return false;
}


float UTGOR_ActionTask::GetInputValue(TSubclassOf<UTGOR_Input> InputType) const
{
	float Value = 0.0f;
	for (const auto& Pair : Inputs.Inputs)
	{
		if (IsValid(Pair.Key) && Pair.Key->IsA(InputType))
		{
			Value = FMath::Max(Value, Pair.Value);
		}
	}
	return Value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UActorComponent* UTGOR_ActionTask::GetAimedComponent() const
{
	return LastAim.Component.Get();
}

UTGOR_Target* UTGOR_ActionTask::GetAimedTarget() const
{
	return LastAim.Target;
}

FVector UTGOR_ActionTask::GetAimedLocation(bool Sticky) const
{
	// Make sure component is either valid or undefined
	if (IsValid(LastAim.Target) && LastAim.Component.IsValid())
	{
		if (Sticky)
		{
			return(LastAim.Target->QueryStickyLocation(LastAim));
		}
		return(LastAim.Target->QueryAimLocation(LastAim));
	}

	// Default to worldspace if there is no target
	return(LastAim.Offset);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_ActionTask::HasNoCooldown() const
{
	// Check cooldown
	if (GetTimeSinceLastCall() >= Identifier.Content->ScheduleCooldown)
	{
		return true;
	}
	return false;
}

bool UTGOR_ActionTask::HasValidTarget() const
{
	if (Identifier.Content->AimTarget == ETGOR_AimIgnoreEnumeration::None)
	{
		return true;
	}

	if (LastAim.Component.IsValid() && Identifier.Content->Instanced_TargetInsertions.Contains(LastAim.Target))
	{
		if (Identifier.Content->AimTarget == ETGOR_AimIgnoreEnumeration::IgnoreSelf)
		{
			return (LastAim.Component->GetOwner() != Identifier.Component->GetOwner());
		}
		else if (Identifier.Content->AimTarget == ETGOR_AimIgnoreEnumeration::OnlySelf)
		{
			return (LastAim.Component->GetOwner() == Identifier.Component->GetOwner());
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool UTGOR_ActionTask::HasValidMovement() const
{
	// If there is no movement we don't restrict
	if (!MovementComponent.IsValid())
	{
		return true;
	}

	// Not running any movement
	UTGOR_MovementTask* CurrentTask = MovementComponent->GetMovementTask();
	if (!IsValid(CurrentTask))
	{
		return false;
	}

	// If there are no restrictions we allow all movements but issue a warning.
	// Actions that don't require a movement check on a character with movement are almost always a sign they have been forgotten.
	if (!ensureMsgf(Identifier.Content->Instanced_MovementInsertions.Collection.Num() > 0, TEXT("There is movement but no restriction defined. Please explicitely allow all allowed movements.")))
	{
		return true;
	}

	// Check whether movement mode is supported
	if (ETGOR_MovementRestrict* Ptr = Identifier.Content->Instanced_MovementInsertions.Collection.Find(CurrentTask->GetMovement()))
	{
		return (*Ptr == ETGOR_MovementRestrict::Always) || IsRunning();
	}
	return false;
}

bool UTGOR_ActionTask::IsInRange() const
{
	if (Identifier.Content->AimRange == ETGOR_AimDistanceEnumeration::NoRange)
	{
		return true;
	}

	if (PilotComponent.IsValid())
	{
		// We use some linear combination of the shape radius/height for range checks
		const FTGOR_MovementShape& Shape = PilotComponent->GetBody();
		const float SquareShape = (Shape.Radius * Shape.Radius + Shape.Height * Shape.Height) / 4;

		const int32 SquareReach = SquareShape * FMath::Square(Identifier.Content->AimReachDistance);
		const int32 SquareRange = SquareShape * FMath::Square(Identifier.Content->AimRangeDistance);

		const FVector AimLocation = GetAimedLocation(true);
		const FTGOR_MovementPosition Position = PilotComponent->ComputePosition();
		const float SquareDistance = (AimLocation - Position.Linear).SizeSquared();

		if (Identifier.Content->AimRange == ETGOR_AimDistanceEnumeration::InRange)
		{
			return (SquareDistance < SquareRange);
		}
		else if (Identifier.Content->AimRange == ETGOR_AimDistanceEnumeration::InReach)
		{
			return (SquareDistance < SquareReach);
		}
		else
		{
			return (SquareReach < SquareDistance) && (SquareDistance < SquareRange);
		}
	}
	return false;
}

bool UTGOR_ActionTask::CheckOwnerState() const
{
	return (HasNoCooldown() || IsRunning()) && HasValidTarget() && HasValidMovement() && IsInRange();
}

bool UTGOR_ActionTask::CollectDebugInfo(float LogDuration, FTGOR_ActionDebugInfo& DebugInfo) const
{
	if (IsValid(Identifier.Content) && IsValid(Identifier.Component) && Identifier.Content->Verbose)
	{
		DebugInfo.Action = Identifier.Content;
		DebugInfo.Aim = LastAim;
		DebugInfo.Inputs = Inputs.Inputs;
		DebugInfo.HasNoCooldown = HasNoCooldown();
		DebugInfo.HasValidTarget = HasValidTarget();
		DebugInfo.HasValidMovement = HasValidMovement();
		DebugInfo.IsInRange = IsInRange();

		ETGOR_ValidEnumeration Invariance = ETGOR_ValidEnumeration::Invalid;
		if (CheckOwnerState())
		{
			OnCondition(Invariance);
		}
		DebugInfo.HasCondition = (Invariance == ETGOR_ValidEnumeration::Valid);
		DebugInfo.IsRunning = IsRunning();

		DebugInfo.Logs.Empty();

		if (Singleton.IsValid())
		{
			const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
			for (const FTGOR_ActionLog& DebugLog : DebugLogs)
			{
				const float Time = Timestamp - DebugLog.Timestamp;
				if (Time < LogDuration)
				{
					DebugInfo.Logs.Emplace(FString::Printf(TEXT("%.0fs / %s : %s"), Time, *DebugLog.Domain, *DebugLog.Message));
				}
			}
		}
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionTask::Context(const FTGOR_AimInstance& Aim)
{
	const bool HasChanged = (Aim.Component != LastAim.Component || Aim.Target != LastAim.Target);

	LastAim = Aim;
	OnContext();

	if (HasChanged)
	{
		OnTarget();
	}

	if (Identifier.Content->AutoTrigger && !Identifier.Component->IsRunningAny() && CanCall())
	{
		Identifier.Component->ScheduleSlotAction(Identifier.Slot);
	}
}

void UTGOR_ActionTask::Prepare(FTGOR_ActionState& State)
{
	// Set new slot properties
	SINGLETON_CHK;
	State.Aim = LastAim;
	State.Time = Singleton->GetGameTimestamp();
	State.State = ETGOR_ActionStateEnumeration::Suspended;
	State.ActiveSlot = Identifier.Slot;

	LogActionMessage("Prepare", "Starting action");
}


bool UTGOR_ActionTask::Update(FTGOR_ActionState& State, float Deltatime)
{
	SINGLETON_RETCHK(false);
	if (!IsValid(Identifier.Content)) ERRET("Update Called on invalid action!", Error, false);
	if (!IsValid(Identifier.Component)) ERRET("Update Called on non-instanced action!", Error, false);
	const bool IsServer = Identifier.Component->IsServer();

	//Inputs = State.Inputs;
	//LastAim = State.Aim;

	// Operate depending on current mode (Breaks make the action kill(interupt) itself)
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
	switch (State.State)
	{
	case ETGOR_ActionStateEnumeration::Suspended:
	{
		// Don't forward automatically if enabled
		if (!IsServer && Identifier.Content->NetworkNotify <= ETGOR_NotifyReplication::NoForward)
		{
			return Forward(State, State.State);
		}

		if (!Forward(State, ETGOR_ActionStateEnumeration::Prepare))
		{
			break;
		}
	}

	case ETGOR_ActionStateEnumeration::Prepare:
	{
		// Equip until timer runs out or returned false
		const float Time = GameTimestamp - State.Time;
		const float Progress = UTGOR_Math::SafeDiv(Time, Identifier.Content->MaxPrepareTime);
		bool Equipped = PrepareState(FMath::Clamp(Progress, 0.0f, 1.0f), Deltatime);
		if (!Equipped || Time > Identifier.Content->MaxPrepareTime)
		{
			// Don't forward automatically if enabled
			if (!IsServer && Identifier.Content->NetworkNotify <= ETGOR_NotifyReplication::NoForward && Identifier.Content->ClientAuthority != ETGOR_ClientAuthority::SkipAll)
			{
				return Forward(State, State.State);
			}

			if (!Forward(State, ETGOR_ActionStateEnumeration::Operate))
			{
				break;
			}
		}
		else
		{
			// Wait in this mode
			return Forward(State, State.State);
		}
	}

	case ETGOR_ActionStateEnumeration::Operate:
	{
		if (!Condition())
		{
			break;
		}

		// Operate until returns false
		const float Time = GameTimestamp - State.Time;
		bool IsOperating = OperateState(Time, Deltatime);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(State))
		{
			return false;
		}

		if (!IsOperating)
		{
			// Don't forward automatically if enabled
			if (!IsServer && Identifier.Content->NetworkNotify <= ETGOR_NotifyReplication::NoForward && Identifier.Content->ClientAuthority == ETGOR_ClientAuthority::NoAuthority)
			{
				return Forward(State, State.State);
			}

			if (!Forward(State, ETGOR_ActionStateEnumeration::Finish))
			{
				break;
			}
		}
		else
		{
			// Wait in this mode
			return Forward(State, State.State);
		}
	}

	case ETGOR_ActionStateEnumeration::Finish:
	{
		if (!Condition())
		{
			break;
		}

		// Unequip until timer runs out or returned false
		const float Time = GameTimestamp - State.Time;
		const float Progress = UTGOR_Math::SafeDiv(Time, Identifier.Content->MaxPrepareTime);
		bool Unequipped = FinishState(FMath::Clamp(Progress, 0.0f, 1.0f), Deltatime);
		if (!Unequipped || Time > Identifier.Content->MaxFinishTime)
		{
			// Don't forward automatically if enabled
			if (!IsServer && Identifier.Content->NetworkNotify <= ETGOR_NotifyReplication::NoForward && Identifier.Content->ClientAuthority != ETGOR_ClientAuthority::SkipAll)
			{
				return Forward(State, State.State);
			}

			if (!Forward(State, ETGOR_ActionStateEnumeration::Dead))
			{
				break;
			}
		}

		// Wait in this mode
		return Forward(State, State.State);
	}

	default:
	{
	}
	}

	// Keep action running for clients if enabled
	if (!IsServer && Identifier.Content->NetworkNotify <= ETGOR_NotifyReplication::NoValidation)
	{
		return true;
	}

	Finish(State);
	return false;
}

bool UTGOR_ActionTask::Forward(FTGOR_ActionState& State, ETGOR_ActionStateEnumeration TargetState)
{
	if (!IsValid(Identifier.Component))
	{
		return false;
	}

	// Make sure we only forward if necessary
	if (State.State >= TargetState)
	{
		return State.State == TargetState;
	}

	SINGLETON_RETCHK(false);
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

	// Activate if not yet running
	if (State.State <= ETGOR_ActionStateEnumeration::Suspended && ETGOR_ActionStateEnumeration::Suspended < TargetState)
	{
		if (!Condition())
		{
			LogActionMessage("Forward", "Condition false");
			return false;
		}

		PrepareStart();

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(State))
		{
			return false;
		}

		State.Time = GameTimestamp;
		State.State = ETGOR_ActionStateEnumeration::Prepare;

		LogActionMessage("Forward", "Prepare");
	}

	// Skip equip
	if (State.State <= ETGOR_ActionStateEnumeration::Prepare && ETGOR_ActionStateEnumeration::Prepare < TargetState)
	{
		if (!Condition())
		{
			LogActionMessage("Forward", "Condition false");
			return false;
		}

		OperateStart();

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(State))
		{
			return false;
		}

		State.Time = GameTimestamp;
		State.State = ETGOR_ActionStateEnumeration::Operate;

		LogActionMessage("Forward", "Operate");
	}

	// Skip operate
	if (State.State <= ETGOR_ActionStateEnumeration::Operate && ETGOR_ActionStateEnumeration::Operate < TargetState)
	{
		if (!Condition())
		{
			LogActionMessage("Forward", "Condition false");
			return false;
		}

		FinishStart();

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(State))
		{
			return false;
		}

		State.Time = GameTimestamp;
		State.State = ETGOR_ActionStateEnumeration::Finish;
		LogActionMessage("Forward", "Finish");
	}

	// Skip unequip
	if (State.State <= ETGOR_ActionStateEnumeration::Finish && ETGOR_ActionStateEnumeration::Finish < TargetState)
	{
		Interrupt();

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(State))
		{
			return false;
		}

		State.Time = GameTimestamp;
		State.State = ETGOR_ActionStateEnumeration::Dead;
		State.ActiveSlot = INDEX_NONE;
		LogActionMessage("Forward", "Death");

	}

	// Update state
	State.State = TargetState;
	return State.ActiveSlot != INDEX_NONE;
}

void UTGOR_ActionTask::Finish(FTGOR_ActionState& State)
{
	LogActionMessage("Finish", "Forced Interrupt");
	Forward(State, ETGOR_ActionStateEnumeration::Dead);

	// Forwarding doesn't make sure we're invalidated (e.g. through Condition)
	if (IsRunningIn(State))
	{
		// Force death if forwarding failed
		if (State.State < ETGOR_ActionStateEnumeration::Dead)
		{
			Interrupt();
			State.State = ETGOR_ActionStateEnumeration::Dead;
		}
	}

	State.ActiveSlot = INDEX_NONE;
}


void UTGOR_ActionTask::Integrate(FTGOR_ActionState& State, const FTGOR_ActionState& TargetState)
{
	LogActionMessage("Integrate", "Forward");

	// Update action state
	if (TargetState.State > State.State)
	{
		if (Identifier.Content->ClientAuthority == ETGOR_ClientAuthority::SkipAll)
		{
			Forward(State, TargetState.State);
		}
		else if (Identifier.Content->ClientAuthority == ETGOR_ClientAuthority::SkipOperate)
		{
			// Step forward after operate if client state is ahead
			if (State.State == ETGOR_ActionStateEnumeration::Operate)
			{
				Forward(State, ETGOR_ActionStateEnumeration::Finish);
			}
		}
	}
}

void UTGOR_ActionTask::Animate(float Deltatime)
{
	OnAnimate(Deltatime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
