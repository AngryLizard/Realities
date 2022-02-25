#include "TGOR_ActionStorage.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Unlocks/Modifiers/Actions/TGOR_Action.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

void UTGOR_ActionStorage::Clear()
{
	Super::Clear();
	Time = FTGOR_Time();
	LocalState = ETGOR_ActionStateEnumeration::Suspended;
	TargetState = LocalState;
	Identifier = INDEX_NONE;
	MarkDirty();
}

FString UTGOR_ActionStorage::Print() const
{
	return FString::Printf(TEXT("Action = %s, Time = %s, LocalState = %d, TargetState = %d, Identifier = %d, %s"), *ActionRef->GetDefaultName(), *Time.Print(), (int32)LocalState, (int32)TargetState, Identifier, *Super::Print());
}

bool UTGOR_ActionStorage::Compatible(const UTGOR_Container* Other) const
{
	if (!Super::Compatible(Other))
	{
		return false;
	}

	const UTGOR_ActionStorage* OtherStorage = Cast<UTGOR_ActionStorage>(Other);
	return ActionRef == OtherStorage->ActionRef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ActionStorage::UTGOR_ActionStorage()
:	Time(),
	LocalState(ETGOR_ActionStateEnumeration::Suspended),
	TargetState(ETGOR_ActionStateEnumeration::Suspended),
	Identifier(INDEX_NONE),
	ActionRef(nullptr)
{
}


void UTGOR_ActionStorage::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);
	Package.WriteEntry("Time", Time);
	Package.WriteEntry("ActionState", LocalState);
	Package.WriteEntry("Identifier", Identifier);
}

bool UTGOR_ActionStorage::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);
	Package.ReadEntry("Time", Time);
	Package.ReadEntry("ActionState", TargetState);
	Package.ReadEntry("Identifier", Identifier);
	return true;
}

void UTGOR_ActionStorage::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Send_Implementation(Package, Context);
	Package.WriteEntry(Time);
	Package.WriteEntry(LocalState);
	Package.WriteEntry(Identifier);
}

bool UTGOR_ActionStorage::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Recv_Implementation(Package, Context);
	Package.ReadEntry(Time);
	Package.ReadEntry(TargetState);
	Package.ReadEntry(Identifier);
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActionStorage::Equals(const UTGOR_Storage* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_ActionStorage* OtherStorage = Cast<UTGOR_ActionStorage>(Other);
	if (LocalState != OtherStorage->LocalState) return false;
	if (Identifier != OtherStorage->Identifier) return false;

	return true;
}

bool UTGOR_ActionStorage::Merge(const UTGOR_Storage* Other, ETGOR_NetvarEnumeration Priority)
{
	if (Super::Merge(Other, Priority))
	{
		const UTGOR_ActionStorage* OtherStorage = Cast<UTGOR_ActionStorage>(Other);
		if (Priority != ETGOR_NetvarEnumeration::Client)
		{
			if (OtherStorage->LocalState > LocalState)
			{
				LocalState = OtherStorage->LocalState;
			}

			if (OtherStorage->Identifier != INDEX_NONE)
			{
				Identifier = OtherStorage->Identifier;
			}
		}
		if (Priority == ETGOR_NetvarEnumeration::Local && OtherStorage->Time < Time)
		{
			Time = OtherStorage->Time;
		}
		return true;
	}
	return false;
}

UTGOR_Content* UTGOR_ActionStorage::GetOwner() const
{
	return GetAction();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Action* UTGOR_ActionStorage::GetAction() const
{
	return ActionRef;
}

ETGOR_ActionStateEnumeration UTGOR_ActionStorage::GetState() const
{
	return TargetState;
}

int32 UTGOR_ActionStorage::GetSlotIdentifier() const
{
	return Identifier;
}

float UTGOR_ActionStorage::GetDuration() const
{
	if (IsValid(Singleton))
	{
		return FMath::Max(0.0f, Singleton->GetGameTimestamp() - Time);
	}
	return 0.0f;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemStorage* UTGOR_ActionStorage::GetCurrentItem(UTGOR_ActionComponent* Component) const
{
	if (IsValid(Component))
	{
		return Component->GetItemStorage(Identifier);
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_ActionStorage::SwapWithCurrentItem(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Component))
	{
		return Component->SwapItemStorage(Identifier, Storage);
	}
	return Storage;
}

UTGOR_ItemStorage* UTGOR_ActionStorage::TakeCurrentItem(UTGOR_ActionComponent* Component)
{
	if (IsValid(Component))
	{
		return Component->SwapItemStorage(Identifier, nullptr);
	}
	return nullptr;
}

bool UTGOR_ActionStorage::HasMatter(UTGOR_ActionComponent* Component, UTGOR_Matter* Matter, int32 Quantity) const
{
	UTGOR_ItemStorage* Storage = GetCurrentItem(Component);
	if (IsValid(Storage))
	{
		return Storage->CountMatter(Matter) >= Quantity;
	}
	return Quantity <= 0;
}

int32 UTGOR_ActionStorage::DepleteMatter(UTGOR_ActionComponent* Component, UTGOR_Matter* Matter, int32 Quantity)
{
	if (IsValid(Component))
	{
		UTGOR_ItemStorage* Storage = GetCurrentItem(Component);
		if (IsValid(Storage))
		{
			const int32 Removed = Storage->RemoveMatter(Matter, Quantity);
			if (!Storage->HasAnyMatter())
			{
				// Destroy if fully depleted
				Component->DestroyItemStorage(TakeCurrentItem(Component));
			}
			return Removed;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_ActionStorage::Prepare(UTGOR_ActionComponent* Component, int32 SlotIdentifier)
{
	// Set new slot properties
	SINGLETON_CHK;
	Time = Singleton->GetGameTimestamp();
	TargetState = ETGOR_ActionStateEnumeration::Suspended;
	LocalState = TargetState;
	Identifier = SlotIdentifier;
	MarkDirty();
}


bool UTGOR_ActionStorage::IsRunningIn(UTGOR_ActionComponent* Component) const
{
	return Component->ActiveSlot == Identifier;
}

bool UTGOR_ActionStorage::Update(UTGOR_ActionComponent* Component, float Deltatime)
{
	SINGLETON_RETCHK(false);
	if (!IsValid(Component)) return false;
	const bool IsServer = Component->IsServer();
	
	// Operate depending on current mode (Breaks make the action kill(interupt) itself)
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
	switch (LocalState)
	{
		case ETGOR_ActionStateEnumeration::Suspended:
		{
			// Don't forward automatically if enabled
			if (!IsServer && ActionRef->NetworkNotify <= ETGOR_NotifyReplication::NoForward)
			{
				return Forward(Component, TargetState);
			}

			if (!Forward(Component, ETGOR_ActionStateEnumeration::Prepare))
			{
				break;
			}
		}

		case ETGOR_ActionStateEnumeration::Prepare:
		{
			// Equip until timer runs out or returned false
			bool Equipped = ActionRef->PrepareState(this, Component, Deltatime);
			if (!Equipped || (GameTimestamp - Time) > ActionRef->MaxPrepareTime)
			{
				// Don't forward automatically if enabled
				if (!IsServer && ActionRef->NetworkNotify <= ETGOR_NotifyReplication::NoForward && ActionRef->ClientAuthority != ETGOR_ClientAuthority::SkipAll)
				{
					return Forward(Component, TargetState);
				}

				if (!Forward(Component, ETGOR_ActionStateEnumeration::Operate))
				{
					break;
				}
			}
			else
			{
				// Wait in this mode
				return Forward(Component, TargetState);
			}
		}

		case ETGOR_ActionStateEnumeration::Operate:
		{
			if (!ActionRef->Invariant(this, Component))
			{
				break;
			}

			// Operate until returns false
			bool IsOperating = ActionRef->OperateState(this, Component, Deltatime);

			// Cancel completely if this action got interrupted
			if (!IsRunningIn(Component))
			{
				return false;
			}

			if (!IsOperating)
			{
				// Don't forward automatically if enabled
				if (!IsServer && ActionRef->NetworkNotify <= ETGOR_NotifyReplication::NoForward && ActionRef->ClientAuthority == ETGOR_ClientAuthority::NoAuthority)
				{
					return Forward(Component, TargetState);
				}

				if (!Forward(Component, ETGOR_ActionStateEnumeration::Finish))
				{
					break;
				}
			}
			else
			{
				// Wait in this mode
				return Forward(Component, TargetState);
			}
		}

		case ETGOR_ActionStateEnumeration::Finish:
		{
			if (!ActionRef->Invariant(this, Component))
			{
				break;
			}

			// Unequip until timer runs out or returned false
			bool Unequipped = ActionRef->FinishState(this, Component, Deltatime);
			if (!Unequipped || (GameTimestamp - Time) > ActionRef->MaxFinishTime)
			{
				// Don't forward automatically if enabled
				if (!IsServer && ActionRef->NetworkNotify <= ETGOR_NotifyReplication::NoForward && ActionRef->ClientAuthority != ETGOR_ClientAuthority::SkipAll)
				{
					return Forward(Component, TargetState);
				}

				if (!Forward(Component, ETGOR_ActionStateEnumeration::Dead))
				{
					break;
				}
			}

			// Wait in this mode
			return Forward(Component, TargetState);
		}

		default:
		{
		}
	}

	// Keep action running for clients if enabled
	if (!IsServer && ActionRef->NetworkNotify <= ETGOR_NotifyReplication::NoValidation)
	{
		return true;
	}

	Interrupt(Component);
	return false;
}

bool UTGOR_ActionStorage::Forward(UTGOR_ActionComponent* Component, ETGOR_ActionStateEnumeration State)
{
	TargetState = State;

	if (!IsValid(Component))
	{
		return false;
	}

	// Make sure we only forward if necessary
	if (LocalState >= TargetState)
	{
		return LocalState == TargetState;
	}
	
	SINGLETON_RETCHK(false);
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

	// Activate if not yet running
	if (LocalState <= ETGOR_ActionStateEnumeration::Suspended && ETGOR_ActionStateEnumeration::Suspended < TargetState)
	{
		if (!ActionRef->Invariant(this, Component))
		{
			ActionRef->LogActionMessage("Inerrupted from Suspended", Component);
			return false;
		}

		ActionRef->PrepareStart(this, Component);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(Component))
		{
			return false;
		}

		Time = GameTimestamp;
		LocalState = ETGOR_ActionStateEnumeration::Prepare;
		MarkDirty();

		// Replicate in case Activate changed something
		if (HasNotify())
		{
			Replicate(Component);
		}

		ActionRef->LogActionMessage("Forwarded Activation", Component);
	}

	// Skip equip
	if (LocalState <= ETGOR_ActionStateEnumeration::Prepare && ETGOR_ActionStateEnumeration::Prepare < TargetState)
	{
		if (!ActionRef->Invariant(this, Component))
		{
			ActionRef->LogActionMessage("Interrupted from Prepare", Component);
			return false;
		}

		ActionRef->OperateStart(this, Component);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(Component))
		{
			return false;
		}

		Time = GameTimestamp;
		LocalState = ETGOR_ActionStateEnumeration::Operate;
		MarkDirty();

		// Replicate in case ForceEquip changed something
		if (HasNotify())
		{
			Replicate(Component);
		}

		ActionRef->LogActionMessage("Forwarded Equip", Component);
	}

	// Skip operate
	if (LocalState <= ETGOR_ActionStateEnumeration::Operate && ETGOR_ActionStateEnumeration::Operate < TargetState)
	{
		if (!ActionRef->Invariant(this, Component))
		{
			ActionRef->LogActionMessage("Interrupted from Operate", Component);
			return false;
		}

		ActionRef->FinishStart(this, Component);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(Component))
		{
			return false;
		}

		Time = GameTimestamp;
		LocalState = ETGOR_ActionStateEnumeration::Finish;
		MarkDirty();

		// Replicate in case ForceEquip changed something
		if (HasNotify())
		{
			Replicate(Component);
		}

		ActionRef->LogActionMessage("Forwarded Operate", Component);
	}

	// Skip unequip
	if (LocalState <= ETGOR_ActionStateEnumeration::Finish && ETGOR_ActionStateEnumeration::Finish < TargetState)
	{
		// Reset action
		ActionRef->Interrupt(this, Component);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(Component))
		{
			return false;
		}

		LocalState = ETGOR_ActionStateEnumeration::Dead;
		MarkDirty();

		// *always* send data one last time in case something important happened
		Replicate(Component);

		ActionRef->LogActionMessage("Forwarded Death", Component);

		// Invalidate trigger
		Identifier = INDEX_NONE;
		Component->ActionState.Action.Storage = nullptr;
	}
	
	// Update state
	LocalState = TargetState;
	return Identifier != INDEX_NONE;
}

void UTGOR_ActionStorage::Interrupt(UTGOR_ActionComponent* Component)
{
	Forward(Component, ETGOR_ActionStateEnumeration::Dead);

	// Forwarding doesn't make sure storage is removed (e.g. invariant)
	if (IsRunningIn(Component))
	{
		// Force death if forwarding failed
		if (LocalState < ETGOR_ActionStateEnumeration::Dead)
		{
			ActionRef->Interrupt(this, Component);
			LocalState = ETGOR_ActionStateEnumeration::Dead;
		}

		Identifier = INDEX_NONE;
		Component->ActionState.Action.Storage = nullptr;
	}


	ActionRef->LogActionMessage("Interrupted", Component);
}


void UTGOR_ActionStorage::Replicate(UTGOR_ActionComponent* Component)
{
	if (!IsValid(Component)) return;

	// If any variables got changed, send/replicate them
	const ENetRole NetRole = Component->GetOwnerRole();
	if (NetRole == ENetRole::ROLE_AutonomousProxy)
	{
		if (ActionRef->IsReliable())
		{
			Component->SendVariablesReliable(this);
		}
		else
		{
			Component->SendVariablesUnreliable(this);
		}
	}
	else if (NetRole == ENetRole::ROLE_Authority)
	{
		// Make sure clients are updated
		Component->ActionState.Item.Storage = Component->GetItemStorage(Identifier);
		Component->AdjustClientActionTrigger(Component->ActionState);
	}
}

void UTGOR_ActionStorage::Integrate(UTGOR_ActionComponent* Component, UTGOR_ActionStorage* Storage)
{
	if (Storage->TargetState > LocalState)
	{
		if (ActionRef->ClientAuthority == ETGOR_ClientAuthority::SkipAll)
		{
			TargetState = Storage->TargetState;
			// We don't immediately forward since e.g. events depend on tick running again at least once
			// Forward(Component, Storage->TargetState);
		}
		else if (ActionRef->ClientAuthority == ETGOR_ClientAuthority::SkipOperate)
		{
			// Step forward after operate if client state is ahead
			if (TargetState == ETGOR_ActionStateEnumeration::Operate)
			{
				TargetState = ETGOR_ActionStateEnumeration::Finish;
				// We don't immediately forward since e.g. events depend on tick running again at least once
				// Forward(Component, ETGOR_ActionStateEnumeration::Finish);
			}
		}
	}

	Merge(Storage, ETGOR_NetvarEnumeration::Client);

	MarkDirty();
}