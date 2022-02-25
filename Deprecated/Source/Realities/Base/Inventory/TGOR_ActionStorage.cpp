#include "TGOR_ActionStorage.h"

#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

void UTGOR_ActionStorage::Clear()
{
	Super::Clear();
	Time = FTGOR_Time();
	LocalState = ETGOR_ActionStateEnumeration::Suspended;
	TargetState = LocalState;
	SlotIndex = FTGOR_SlotIndex();
	MarkDirty();
}

FString UTGOR_ActionStorage::Print() const
{
	return FString::Printf(TEXT("Action = %s, Time = %s, LocalState = %d, TargetState = %d, SlotIndex = %s, %s"), *ActionRef->GetDefaultName(), *Time.Print(), (int32)LocalState, (int32)TargetState, *SlotIndex.Print(), *Super::Print());
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
	SlotIndex(),
	ActionRef(nullptr)
{
}


void UTGOR_ActionStorage::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);
	Package.WriteEntry("Time", Time);
	Package.WriteEntry("ActionState", LocalState);
	Package.WriteEntry("SlotIndex", SlotIndex);
}

bool UTGOR_ActionStorage::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);
	Package.ReadEntry("Time", Time);
	Package.ReadEntry("ActionState", TargetState);
	Package.ReadEntry("SlotIndex", SlotIndex);
	return true;
}

void UTGOR_ActionStorage::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Send_Implementation(Package, Context);
	Package.WriteEntry(Time);
	Package.WriteEntry(LocalState);
	Package.WriteEntry(SlotIndex);
}

bool UTGOR_ActionStorage::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Recv_Implementation(Package, Context);
	Package.ReadEntry(Time);
	Package.ReadEntry(TargetState);
	Package.ReadEntry(SlotIndex);
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
	if (SlotIndex != OtherStorage->SlotIndex) return false;

	return true;
}

bool UTGOR_ActionStorage::Merge(const UTGOR_Storage* Other, ETGOR_NetvarEnumeration Priority)
{
	if (Super::Merge(Other, Priority))
	{
		const UTGOR_ActionStorage* OtherStorage = Cast<UTGOR_ActionStorage>(Other);
		if (OtherStorage->LocalState > LocalState)
		{
			LocalState = OtherStorage->LocalState;
		}
		if (Priority == ETGOR_NetvarEnumeration::Local && OtherStorage->Time < Time)
		{
			Time = OtherStorage->Time;
		}
		if (OtherStorage->SlotIndex.Valid)
		{
			SlotIndex = OtherStorage->SlotIndex;
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

FTGOR_SlotIndex UTGOR_ActionStorage::GetSlot() const
{
	return SlotIndex;
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
		return Component->GetItemStorage(SlotIndex);
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_ActionStorage::SwapWithCurrentItem(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Component))
	{
		return Component->SwapItemStorage(SlotIndex, Storage);
	}
	return Storage;
}

UTGOR_ItemStorage* UTGOR_ActionStorage::TakeCurrentItem(UTGOR_ActionComponent* Component)
{
	if (IsValid(Component))
	{
		return Component->SwapItemStorage(SlotIndex, nullptr);
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


void UTGOR_ActionStorage::Prepare(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Index)
{
	// Set new slot properties
	SINGLETON_CHK;
	Time = Singleton->GetGameTimestamp();
	TargetState = ETGOR_ActionStateEnumeration::Suspended;
	LocalState = TargetState;
	SlotIndex = Index;
	MarkDirty();
}


bool UTGOR_ActionStorage::IsRunningIn(UTGOR_ActionComponent* Component) const
{
	return Component->ActionState.Action.Storage == this;
}

bool UTGOR_ActionStorage::Wait(UTGOR_ActionComponent* Component, float Deltatime)
{
	if (Forward(Component, TargetState))
	{
		Replicate(Component, false); // Replication happens before tick for cleanup, some modules depends on this
		Tick(Component, Deltatime);
		return true;
	}
	return false;
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
			if (!IsServer && ActionRef->Notify <= ETGOR_NotifyReplication::NoForward)
			{
				return Wait( Component, Deltatime);
			}

			if (!Forward(Component, ETGOR_ActionStateEnumeration::Equip))
			{
				break;
			}
		}

		case ETGOR_ActionStateEnumeration::Equip:
		{
			// Equip until timer runs out or returned false
			bool Equipped = ActionRef->Equip(this, Component, Deltatime);
			if (!Equipped || (GameTimestamp - Time) > ActionRef->MaxEquipTime)
			{
				// Don't forward automatically if enabled
				if (!IsServer && ActionRef->Notify <= ETGOR_NotifyReplication::NoForward)
				{
					return Wait(Component, Deltatime);
				}

				if (!Forward(Component, ETGOR_ActionStateEnumeration::Operate))
				{
					break;
				}
			}
			else
			{
				// Wait in this mode
				return Wait(Component, Deltatime);
			}
		}

		case ETGOR_ActionStateEnumeration::Operate:
		{
			if (!ActionRef->Invariant(this, Component))
			{
				break;
			}

			// Operate until returns false
			bool IsOperating = ActionRef->Operate(this, Component, Deltatime);

			// Cancel completely if this action got interrupted
			if (!IsRunningIn(Component))
			{
				return false;
			}

			if (!IsOperating)
			{
				// Don't forward automatically if enabled
				if (!IsServer && ActionRef->Notify <= ETGOR_NotifyReplication::NoForward)
				{
					return Wait(Component, Deltatime);
				}

				if (!Forward(Component, ETGOR_ActionStateEnumeration::Unequip))
				{
					break;
				}
			}
			else
			{
				// Wait in this mode
				return Wait(Component, Deltatime);
			}
		}

		case ETGOR_ActionStateEnumeration::Unequip:
		{
			if (!ActionRef->Invariant(this, Component))
			{
				break;
			}

			// Unequip until timer runs out or returned false
			bool Unequipped = ActionRef->Unequip(this, Component, Deltatime);
			if (!Unequipped || (GameTimestamp - Time) > ActionRef->MaxUnequipTime)
			{
				// Don't forward automatically if enabled
				if (!IsServer && ActionRef->Notify <= ETGOR_NotifyReplication::NoForward)
				{
					return Wait(Component, Deltatime);
				}

				if (!Forward(Component, ETGOR_ActionStateEnumeration::Dead))
				{
					break;
				}
			}

			// Wait in this mode
			return Wait(Component, Deltatime);
		}

		default:
		{
		}
	}

	// Keep action running for clients if enabled
	if (!IsServer && ActionRef->Notify <= ETGOR_NotifyReplication::NoValidation)
	{
		return true;
	}

	Interrupt(Component);
	return false;
}

bool UTGOR_ActionStorage::Forward(UTGOR_ActionComponent* Component, ETGOR_ActionStateEnumeration State)
{
	TargetState = State;

	if (!IsValid(Component)) return false;

	// Make sure we only forward if necessary
	if (LocalState >= TargetState) return LocalState == TargetState;
	
	SINGLETON_RETCHK(false);
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

	// Activate if not yet running
	if (LocalState <= ETGOR_ActionStateEnumeration::Suspended && ETGOR_ActionStateEnumeration::Suspended < TargetState)
	{
		if (!ActionRef->Invariant(this, Component))
		{
			return false;
		}

		ActionRef->Activate(this, Component);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(Component))
		{
			return false;
		}

		Time = GameTimestamp;
		LocalState = ETGOR_ActionStateEnumeration::Equip;
		MarkDirty();

		// Replicate in case Activate changed something
		if (HasNotify())
		{
			Replicate(Component, false);
		}

		if (Component->Verbose)
		{
			EOUT(ActionRef->GetDefaultName() + " forwarded activation")
		}
	}

	// Skip equip
	if (LocalState <= ETGOR_ActionStateEnumeration::Equip && ETGOR_ActionStateEnumeration::Equip < TargetState)
	{
		if (!ActionRef->Invariant(this, Component))
		{
			return false;
		}

		ActionRef->ForceEquip(this, Component);

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
			Replicate(Component, false);
		}

		if (Component->Verbose)
		{
			EOUT(ActionRef->GetDefaultName() + " forwarded equip")
		}
	}

	// Skip operate
	if (LocalState <= ETGOR_ActionStateEnumeration::Operate && ETGOR_ActionStateEnumeration::Operate < TargetState)
	{
		LocalState = ETGOR_ActionStateEnumeration::Unequip;
		MarkDirty();

		Time = GameTimestamp;
	}

	// Skip unequip
	if (LocalState <= ETGOR_ActionStateEnumeration::Unequip && ETGOR_ActionStateEnumeration::Unequip < TargetState)
	{
		// Reset action
		ActionRef->ForceUnequip(this, Component);

		// Cancel completely if this action got interrupted
		if (!IsRunningIn(Component))
		{
			return false;
		}

		LocalState = ETGOR_ActionStateEnumeration::Dead;
		MarkDirty();

		// Replicate in case ForceUnequip changed something
		if (HasNotify())
		{
			Replicate(Component, false);
		}

		if (Component->Verbose)
		{
			EOUT(ActionRef->GetDefaultName() + " forwarded death")
		}

		// Send data one last time in case something important happened
		Tick(Component, 0.0f);
		Replicate(Component, false);
		// TODO: Do we tick storage here? I say nay but either is probably fine.

		// Invalidate trigger
		SlotIndex.Valid = false;
		Component->ActionState.Action.Storage = nullptr;
	}
	
	// Update state
	LocalState = TargetState;
	return SlotIndex.Valid;
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
			ActionRef->ForceUnequip(this, Component);
			LocalState = ETGOR_ActionStateEnumeration::Dead;
		}

		SlotIndex.Valid = false;
		Component->ActionState.Action.Storage = nullptr;
	}
}


void UTGOR_ActionStorage::Replicate(UTGOR_ActionComponent* Component, bool Force)
{
	if (!IsValid(Component)) return;

	if (PurgeDirt() || Force)
	{
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
			Component->ActionState.Item.Storage = Component->GetItemStorage(SlotIndex);
			Component->AdjustClientActionTrigger(Component->ActionState);
		}
	}
}

void UTGOR_ActionStorage::Integrate(UTGOR_ActionStorage* Storage)
{
	Merge(Storage, ETGOR_NetvarEnumeration::Client);
	MarkDirty();
}