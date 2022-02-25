// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionComponent.h"

#include "GameFramework/PlayerState.h"
#include "Templates/SubclassOf.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/TGOR_ActionStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_InputModule.h"
#include "Realities/Base/Inventory/Modules/TGOR_EventModule.h"
#include "Realities/Animation/TGOR_AnimInstance.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"

#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Targets/TGOR_Target.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionSlot)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionContainer)
#define ACTION_VERBOSE(T, N) if (Verbose) {EOUT((IsValid(N) ? N->GetAction()->GetName() : "None") + ": " + T)}


FTGOR_ActionTrigger::FTGOR_ActionTrigger()
	: Value(0.0f), Trigger()
{
}

FTGOR_ActionTrigger::FTGOR_ActionTrigger(float Value, TSubclassOf<UTGOR_Action> Trigger)
	: Value(Value), Trigger(Trigger)
{
}


FTGOR_ActionSlot::FTGOR_ActionSlot()
	: Action(nullptr), Timestamp()
{
}

void FTGOR_ActionSlot::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Action);
}

void FTGOR_ActionSlot::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Action);
}


FTGOR_ActionContainer::FTGOR_ActionContainer()
{
}

FTGOR_ActionContainer::FTGOR_ActionContainer(TSubclassOf<UTGOR_Action> Type)
	: Type(Type), LastActive(0)
{
}

void FTGOR_ActionContainer::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Slots);
}

void FTGOR_ActionContainer::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Slots);
}


UTGOR_ActionComponent::UTGOR_ActionComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	AnimationSlotName = "";
	Animation = nullptr;
	Pawn = nullptr;
}

void UTGOR_ActionComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	Pawn = Cast<ATGOR_Pawn>(Owner);

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (const auto& Pair : Triggers)
	{
		TArray<UTGOR_Input*> List = ContentManager->GetTListFromType<UTGOR_Input>(Pair.Key);
		for (UTGOR_Input* Input : List)
		{
			Inputs.Emplace(Input, FTGOR_ActionTrigger(0.0f, Pair.Value));
		}
	}
}

void UTGOR_ActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Update currently scheduled action
	if (IsRunning(ActionState.Action.Storage))
	{
		// Update action
		const float Timestep = UpdateActionTimestamp(ActionState.Action.Storage->GetSlot());
		if (Timestep >= SMALL_NUMBER)
		{
			ActionState.Action.Storage->Update(this, Timestep);
		}
	}
}

void UTGOR_ActionComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ActionComponent, ActionContainers, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UTGOR_ActionComponent, ActionState, COND_SimulatedOnly); // COND_SimulatedOnly / COND_InitialOnly
}

bool UTGOR_ActionComponent::CanStoreItemAt(const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage) const
{
	if (Super::CanStoreItemAt(Index, Storage))
	{
		const FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		if (ActionContainer.Slots.IsValidIndex(Index.Slot))
		{
			const FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Index.Slot];
			return IsValid(ActionSlot.Action) && (!Storage || ActionSlot.Action->SupportsItem(Storage->GetItem()));
		}
	}
	return false;
}

int32 UTGOR_ActionComponent::GetContainerCapacity(const FTGOR_ContainerIndex& Index) const
{
	if (Index.IsValid() && ActionContainers.IsValidIndex(Index.Container))
	{
		return ActionContainers[Index.Container].Slots.Num();
	}
	return 0;
}

TSet<UTGOR_Content*> UTGOR_ActionComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ActiveContent = Super::GetActiveContent_Implementation();
	const FTGOR_SlotIndex Index = GetCurrentActionSlot();

	UTGOR_Action* Action = GetAction(Index);
	if (IsValid(Action))
	{
		ActiveContent.Emplace(Action);
	}

	UTGOR_ItemStorage* Storage = GetItemStorage(Index);
	if (IsValid(Storage))
	{
		ActiveContent.Emplace(Storage->GetItem());
	}

	return ActiveContent;
}

void UTGOR_ActionComponent::UpdateUnlocks_Implementation(UTGOR_UnlockComponent* Component)
{
	Constraints.Reset();
	Constraints.Append(Component->GetUnlockedListFromType(UTGOR_Action::StaticClass()));
}

bool UTGOR_ActionComponent::SupportsContent_Implementation(UTGOR_Content* Content) const
{
	if (Constraints.Num() == 0) return true;
	return Constraints.Contains(Content);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_Pawn* UTGOR_ActionComponent::GetOwnerPawn() const
{
	return Pawn;
}


ATGOR_Avatar* UTGOR_ActionComponent::GetOwnerAvatar(ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;
	ATGOR_Avatar* Avatar = Cast<ATGOR_Avatar>(Pawn);
	if (IsValid(Avatar))
	{
		Branches = ETGOR_BoolEnumeration::Is;
		return Avatar;
	}
	return nullptr;
}

void UTGOR_ActionComponent::BuildActionQueues(UTGOR_Content* Content)
{
	if (IsValid(Content))
	{
		// Clear slots
		for (FTGOR_ActionContainer& Container : ActionContainers)
		{
			Container.Slots.Empty();
		}

		// Try to add all equipables added to creature
		const TArray<UTGOR_Action*> Actions = Content->GetIListFromType<UTGOR_Action>();
		for (UTGOR_Action* Action : Actions)
		{
			ETGOR_ComputeEnumeration State;
			AddSlotAction(Cast<UTGOR_Action>(Action), State);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ActionComponent::GetSlotCooldown(const FTGOR_SlotIndex& Index) const
{
	if (!Singleton) return 0.0f;

	// Check index validity
	if (ActionContainers.IsValidIndex(Index.Container))
	{
		const FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		if (ActionContainer.Slots.IsValidIndex(Index.Slot))
		{
			// Make sure action is valid
			const FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Index.Slot];
			return(Singleton->GetGameTimestamp() - ActionSlot.Timestamp);
		}
	}
	return 0.0f;
}

FTGOR_ContainerIndex UTGOR_ActionComponent::GetContainerTypeIndex(TSubclassOf<UTGOR_Action> Type) const
{
	for (int32 Container = 0; Container < ActionContainers.Num(); Container++)
	{
		const FTGOR_ActionContainer& ActionContainer = ActionContainers[Container];
		if (*ActionContainer.Type && ActionContainer.Type->IsChildOf(Type))
		{
			return FTGOR_ContainerIndex(Container);
		}
	}
	return FTGOR_ContainerIndex();
}


bool UTGOR_ActionComponent::ScheduleContainerType(TSubclassOf<UTGOR_Action> Type)
{
	// Look for container type
	if (ScheduleContainer(GetContainerTypeIndex(Type)))
	{
		return true;
	}

	// Look for fitting action type
	for (int32 Container = 0; Container < ActionContainers.Num(); Container++)
	{
		const FTGOR_ActionContainer& ActionContainer = ActionContainers[Container];
		for (int32 Slot = 0; Slot < ActionContainer.Slots.Num(); Slot++)
		{
			const FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Slot];
			if (IsValid(ActionSlot.Action) && ActionSlot.Action->IsA(Type))
			{
				return ScheduleSlotAction(FTGOR_SlotIndex(Container, Slot));
			}
		}
	}
	return false;
}

int32 UTGOR_ActionComponent::GetContainerSize(const FTGOR_ContainerIndex& Index) const
{
	if (Index.IsValid() && ActionContainers.IsValidIndex(Index.Container))
	{
		const FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		return ActionContainer.Slots.Num();
	}
	return 0;
}

FTGOR_SlotIndex UTGOR_ActionComponent::GetContainerLastSlot(const FTGOR_ContainerIndex& Index) const
{
	if (Index.IsValid() && ActionContainers.IsValidIndex(Index.Container))
	{
		const FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		if (ActionContainer.LastActive.IsValid() && ActionContainer.Slots.IsValidIndex(ActionContainer.LastActive.Slot))
		{
			return ActionContainer.LastActive;
		}
		else
		{
			return FTGOR_SlotIndex(Index, 0);
		}
	}
	return FTGOR_SlotIndex();
}

FTGOR_SlotIndex UTGOR_ActionComponent::GetCurrentActionSlot() const
{
	if (IsValid(ActionState.Action.Storage))
	{
		return ActionState.Action.Storage->GetSlot();
	}
	return FTGOR_SlotIndex();
}

bool UTGOR_ActionComponent::ScheduleContainer(const FTGOR_ContainerIndex& Index)
{
	const FTGOR_SlotIndex LastActive = GetContainerLastSlot(Index);
	if (LastActive.IsValid())
	{
		// Try all slots, starting at LastActive
		const int32 Size = GetContainerSize(Index);
		if (Size > 0)
		{
			for (int32 Slot = 0; Slot < Size; Slot++)
			{
				if (ScheduleSlotAction(FTGOR_SlotIndex(Index, (LastActive.Slot + Slot) % Size)))
				{
					return true;
				}
			}
		}
		ACTION_VERBOSE("Container empty", ActionState.Action.Storage);
	}
	else
	{
		ACTION_VERBOSE("Container index invalid", ActionState.Action.Storage);
	}
	return false;
}

bool UTGOR_ActionComponent::ScheduleSlotAction(const FTGOR_SlotIndex& Index)
{
	if (RunSlotAction(Index, false))
	{
		if (IsServer())
		{
			// Make sure clients are updated
			ActionState.Action.Storage->Replicate(this, true);
		}
		else if (IsLocallyControlled())
		{
			// Send to server if we're in control
			ScheduleActionServer(Index);
		}

		OnActionScheduled.Broadcast(ActionState.Action.Storage->GetAction());
		return true;
	}
	return false;
}

void UTGOR_ActionComponent::SetSlotActive(const FTGOR_SlotIndex& Index)
{
	// Set active slot on a given container
	if (ActionContainers.IsValidIndex(Index.Container))
	{
		ActionContainers[Index.Container].LastActive = Index;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_SlotIndex UTGOR_ActionComponent::AddSlotAction(UTGOR_Action* Action, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (IsValid(Action))
	{
		for (int32 Container = 0; Container < ActionContainers.Num(); Container++)
		{
			FTGOR_ActionContainer& ActionContainer = ActionContainers[Container];
			if (Action->IsA(ActionContainer.Type))
			{
				FTGOR_ActionSlot ActionSlot;
				ActionSlot.Action = Action;
				int32 Slot = ActionContainer.Slots.Add(ActionSlot);

				OnActionUpdated.Broadcast();
				Branches = ETGOR_ComputeEnumeration::Success;
				return FTGOR_SlotIndex(Container, Slot);
			}
		}
	}
	return(FTGOR_SlotIndex());
}

void UTGOR_ActionComponent::SetSlotAction(const FTGOR_SlotIndex& Index, UTGOR_Action* Action, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (Index.IsValid() && ActionContainers.IsValidIndex(Index.Container))
	{
		FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		if (ActionContainer.Slots.IsValidIndex(Index.Slot))
		{
			// Kill action if we are currently running one on this index
			FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Index.Slot];

			const FTGOR_SlotIndex SlotIndex = GetCurrentActionSlot();
			if (IsValid(ActionSlot.Action) && SlotIndex == Index)
			{
				ActionState.Action.Storage->Interrupt(this);
			}

			// Replace action
			ActionSlot.Action = Action;

			// Ensure current slot item is compatible with the new action
			UTGOR_ItemStorage* Storage = GetItemStorage(Index);
			if (!CanStoreItemAt(Index, Storage))
			{
				Storage = SwapItemStorage(Index, nullptr);
			}

			// Put previous back into inventory
			if (IsValid(Pawn))
			{
				Storage = Pawn->PutStorage(Storage);
			}
			DropItemStorage(Storage);

			OnActionUpdated.Broadcast();
		}
	}
}

FTGOR_SlotIndex UTGOR_ActionComponent::RemoveSlotAction(UTGOR_Action* Action, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	if (IsValid(Action))
	{
		for (int Container = 0; Container < ActionContainers.Num(); Container++)
		{
			FTGOR_ActionContainer& ActionContainer = ActionContainers[Container];
			if (Action->IsA(ActionContainer.Type))
			{
				for (int Slot = 0; Slot < ActionContainer.Slots.Num(); Slot++)
				{
					FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Slot];
					if (ActionSlot.Action == Action)
					{
						// Remove action slot
						ActionContainer.Slots.RemoveAt(Slot);

						// Put current item back into inventory
						UTGOR_ItemStorage* Storage = SwapItemStorage(FTGOR_SlotIndex(Container, Slot), nullptr);
						if (IsValid(Pawn))
						{
							Storage = Pawn->PutStorage(Storage);
						}
						DropItemStorage(Storage);

						OnActionUpdated.Broadcast();
						Branches = ETGOR_ComputeEnumeration::Success;
						return FTGOR_SlotIndex(Container, Slot);
					}
				}

			}
		}
	}
	return(FTGOR_SlotIndex());
}


void UTGOR_ActionComponent::RemoveAllSlotActions(TSubclassOf<UTGOR_Action> Type)
{
	for (int Container = 0; Container < ActionContainers.Num(); Container++)
	{
		FTGOR_ActionContainer& ActionContainer = ActionContainers[Container];
		if (Type->IsChildOf(ActionContainer.Type))
		{
			for (int Slot = 0; Slot < ActionContainer.Slots.Num(); Slot++)
			{
				FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Slot];

				if (ActionSlot.Action->IsA(Type))
				{
					ActionContainer.Slots.RemoveAt(Slot);

					// Put current item back into inventory
					UTGOR_ItemStorage* Storage = SwapItemStorage(FTGOR_SlotIndex(Container, Slot), nullptr);
					if (IsValid(Pawn))
					{
						Storage = Pawn->PutStorage(Storage);
					}
					DropItemStorage(Storage);

				}
			}
		}
	}
	OnActionUpdated.Broadcast();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Action* UTGOR_ActionComponent::GetAction(const FTGOR_SlotIndex& Index) const
{
	if (Index.IsValid() && ActionContainers.IsValidIndex(Index.Container))
	{
		const FTGOR_ActionContainer& ActionSlots = ActionContainers[Index.Container];
		if (ActionSlots.Slots.IsValidIndex(Index.Slot))
		{
			return(ActionSlots.Slots[Index.Slot].Action);
		}
	}
	return(nullptr);
}

void UTGOR_ActionComponent::ScheduleEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (IsValid(ActionState.Action.Storage))
	{
		UTGOR_EventModule* EventModule = ActionState.Action.Storage->GetModule<UTGOR_EventModule>();
		if (IsValid(EventModule))
		{
			EventModule->ScheduleEvent(Event);
			Branches = ETGOR_FetchEnumeration::Found;
		}
	}
}

UTGOR_Module* UTGOR_ActionComponent::ScheduleEventParams(TSubclassOf<UTGOR_Event> Event, TSubclassOf<UTGOR_Module> Type, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (IsValid(ActionState.Action.Storage))
	{
		UTGOR_EventModule* EventModule = ActionState.Action.Storage->GetModule<UTGOR_EventModule>();
		if (IsValid(EventModule))
		{
			EventModule->ScheduleEvent(Event);
			if (*Type)
			{
				UTGOR_Module* Module = ActionState.Action.Storage->GetModule(Type);
				if (IsValid(Module))
				{
					Branches = ETGOR_FetchEnumeration::Found;
					return Module;
				}
			}
			else
			{
				Branches = ETGOR_FetchEnumeration::Found;
				return nullptr;
			}
		}
	}
	return nullptr;
}


bool UTGOR_ActionComponent::IsRunning(UTGOR_ActionStorage* Storage) const
{
	return IsValid(Storage) && Storage->GetSlot().IsValid();
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionComponent::PlayAnimation(TSubclassOf<UTGOR_Animation> InAnimation)
{
	if (IsValid(Pawn))
	{
		UTGOR_AnimInstance* Instance = Pawn->GetAnimation();
		if (IsValid(Instance))
		{
			UTGOR_Spawner* Spawner = Pawn->GetSpawnerContent();
			if (*InAnimation && IsValid(Spawner))
			{
				TArray<UTGOR_Animation*> Animations = Spawner->GetIListFromType<UTGOR_Animation>(InAnimation);
				if (Animations.Num() > 0)
				{
					Animation = Animations[0];
					Instance->AssignAnimationInstance(AnimationSlotName, Animation);
				}
			}
			else
			{
				// Reset animation if none is defined
				Animation = nullptr;
				Instance->AssignAnimationInstance(AnimationSlotName, nullptr);
			}
		}
	}
}


UTGOR_Animation* UTGOR_ActionComponent::GetAnimation(TSubclassOf<UTGOR_Animation> Type, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (IsValid(Animation) && Animation->IsA(Type))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return Animation;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_Time UTGOR_ActionComponent::GetServerTime(float Delay)
{
	SINGLETON_RETCHK(FTGOR_Time());

	return(Singleton->GetGameTimestamp() + Delay);
}


float UTGOR_ActionComponent::GetClientPing()
{
	if (!IsValid(Pawn))
	{
		return(0.0f);
	}

	APlayerState* PlayerState = Pawn->GetPlayerState();
	if (!IsValid(PlayerState))
	{
		return(0.0f);
	}

	return(PlayerState->GetPing());
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

void UTGOR_ActionComponent::RepNotifyActionContainers()
{
	OnActionUpdated.Broadcast();
}


void UTGOR_ActionComponent::RepNotifyActionState(FTGOR_ActionState Old)
{
	AdaptToContainer(ActionState, Old.Action.Storage);
}

void UTGOR_ActionComponent::AdaptToContainer(const FTGOR_ActionState& Container, UTGOR_ActionStorage* Previous)
{
	// Update action if not matching with update
	if (IsRunning(Container.Action.Storage))
	{
		const FTGOR_SlotIndex SlotIndex = Container.Action.Storage->GetSlot();
		const ETGOR_ActionStateEnumeration TargetState = Container.Action.Storage->GetState();
		UTGOR_Action* NewAction = Container.Action.Storage->GetAction();

		if (IsValid(Previous))
		{
			const bool IsSameAction = Previous->GetAction() == NewAction && SlotIndex == Previous->GetSlot();

			if (TargetState == ETGOR_ActionStateEnumeration::Dead)
			{
				if (IsSameAction)
				{
					Previous->Interrupt(this);
				}
				// Assumably this action was already killed by the client themselves.
				// TODO: Technically this *could* be bad in case the action on the server did something different and important in its dying breath.
				//		 But we can't just call it since action parameters might be out of date.
				return;
			}

			// Interrupt old action if replacing
			if (!IsSameAction)
			{
				Previous->Interrupt(this);
			}
		}

		if (TargetState == ETGOR_ActionStateEnumeration::Dead)
		{
			// No need to go further if action is already dead
			return;
		}

		// Expand slots as much as necessary
		ActionContainers.SetNum(FMath::Max(SlotIndex.Container + 1, ActionContainers.Num()));
		FTGOR_ActionContainer& ActionContainer = ActionContainers[SlotIndex.Container];
		ActionContainer.Slots.SetNum(FMath::Max(SlotIndex.Slot + 1, ActionContainer.Slots.Num()));
		FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[SlotIndex.Slot];

		// Update action
		ActionSlot.Action = NewAction;

		// If updated action is new, schedule it properly
		if (!IsValid(ActionState.Action.Storage) || !IsValid(Previous) || SlotIndex != Previous->GetSlot())
		{
			// Need to schedule so forwarding works
			RunSlotAction(SlotIndex, true);
		}
		else if (Previous != ActionState.Action.Storage)
		{
			// Reapply non-replicated values from previous state
			ActionState.Action.Storage->Merge(Previous, ETGOR_NetvarEnumeration::Local);
		}

		// TODO: Technically the server sends the action state after execution, but stuff like events need to be sent before that.
		// Make some way to differentiate.

		// Update item storage
		OverrideItemStorage(SlotIndex, Container.Item.Storage);

		if (IsValid(ActionState.Action.Storage))
		{

			// Capture things that got sent immediately such as events/input
			ActionState.Action.Storage->Merge(Container.Action.Storage, ETGOR_NetvarEnumeration::Server);

			// Forward and merge if necessary
			if (ActionState.Action.Storage->Forward(this, TargetState))
			{
				check(ActionState.Action.Storage && "action got terminated but Forwarding still returned true");

				// Update action immediately in case multiple replications come in
				const float Timestep = UpdateActionTimestamp(ActionState.Action.Storage->GetSlot());
				ActionState.Action.Storage->Update(this, Timestep);
			}
		}
	}
	else if (IsValid(Previous))
	{
		Previous->Interrupt(this);
	}
}

bool UTGOR_ActionComponent::RunSlotAction(const FTGOR_SlotIndex& Index, bool Force)
{
	SINGLETON_RETCHK(false);

	// Update currently scheduled action
	UTGOR_Action* Action = GetAction(Index);
	if (IsValid(Action))
	{
		// Check whether this action can be called at all
		if (Force || Action->CanCall(this, Index))
		{
			// Actually schedule action
			UpdateActionTimestamp(Index);
			SetLastActiveActionSlot(Index);

			// Make sure to terminate previous action
			if (IsRunning(ActionState.Action.Storage))
			{
				ActionState.Action.Storage->Interrupt(this);
			}

			// Prepare action storage
			UTGOR_ActionStorage* Storage = Action->CreateStorage();
			if (IsValid(Storage))
			{
				ActionState.Action.Storage = Storage;
				Storage->Prepare(this, Index);
				return true;
			}
			else
			{
				ACTION_VERBOSE("Creating storage failed", ActionState.Action.Storage);
			}
		}
	}
	else
	{
		ACTION_VERBOSE("Slot index invalid", ActionState.Action.Storage);
	}
	return false;
}

void UTGOR_ActionComponent::ScheduleActionServer_Implementation(FTGOR_SlotIndex SlotIndex)
{
	SINGLETON_CHK;
	if (!ScheduleSlotAction(SlotIndex))
	{
		// If scheduling failed, update client
		AdjustClientActionTrigger(ActionState);
	}
}

bool UTGOR_ActionComponent::ScheduleActionServer_Validate(FTGOR_SlotIndex SlotIndex)
{
	return(true);
}

void UTGOR_ActionComponent::AdjustClientActionTrigger_Implementation(FTGOR_ActionState Container)
{
	// Only need to adjust client
	if (IsServer())
	{
		return;
	}
	
	// Update trigger
	AdaptToContainer(Container, ActionState.Action.Storage);
	OnSlotUpdate.Broadcast();
}

void UTGOR_ActionComponent::SendVariablesReliable_Implementation(FTGOR_ActionPayload Params)
{
	// Set storage (Merge operation makes sure only allowed variables are overridden)
	if (IsRunning(ActionState.Action.Storage))
	{
		ActionState.Action.Storage->Integrate(Params.Storage);
		ACTION_VERBOSE("Set Reliable variable", ActionState.Action.Storage)
	}
}

bool UTGOR_ActionComponent::SendVariablesReliable_Validate(FTGOR_ActionPayload Params)
{
	return true;
}


void UTGOR_ActionComponent::SendVariablesUnreliable_Implementation(FTGOR_ActionPayload Params)
{
	// Set storage (Merge operation makes sure only allowed variables are overridden)
	if (IsRunning(ActionState.Action.Storage))
	{
		ActionState.Action.Storage->Integrate(Params.Storage);
		ACTION_VERBOSE("Set Unreliable variable", ActionState.Action.Storage)
	}
}

bool UTGOR_ActionComponent::SendVariablesUnreliable_Validate(FTGOR_ActionPayload Params)
{
	return true;
}

float UTGOR_ActionComponent::UpdateActionTimestamp(const FTGOR_SlotIndex& Index)
{
	// Update cooldown
	if (ActionContainers.IsValidIndex(Index.Container))
	{
		FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		if (ActionContainer.Slots.IsValidIndex(Index.Slot))
		{
			// Figure out how much time has passed since last update
			FTGOR_ActionSlot& ActionSlot = ActionContainer.Slots[Index.Slot];
			const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
			const float Timestep = GameTimestamp - ActionSlot.Timestamp;
			ActionSlot.Timestamp = GameTimestamp;
			return Timestep;
		}
	}
	return 0.0f;
}

void UTGOR_ActionComponent::SetLastActiveActionSlot(const FTGOR_SlotIndex& Index)
{
	// Update cooldown
	if (ActionContainers.IsValidIndex(Index.Container))
	{
		FTGOR_ActionContainer& ActionContainer = ActionContainers[Index.Container];
		if (ActionContainer.Slots.IsValidIndex(Index.Slot) && Index.IsValid())
		{
			ActionContainer.LastActive = Index;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActionComponent::SetInput(TSubclassOf<UTGOR_Input> Input, float Value)
{
	const bool Running = IsRunning(ActionState.Action.Storage);

	// Check input values for whether a trigger has been pulled
	for (auto& Pair : Inputs)
	{
		if (Pair.Key->IsA(Input))
		{
			if (!Pair.Key->IsActive(Pair.Value.Value) && Pair.Key->IsActive(Value))
			{
				if (Running)
				{
					// Only reschedule if trigger doesn't match and the running action doesn't support this input
					UTGOR_Action* Action = ActionState.Action.Storage->GetAction();
					if (!Action->IsA(Pair.Value.Trigger) && !Action->ContainsInsertion(Pair.Key))
					{
						ScheduleContainerType(Pair.Value.Trigger);
					}
				}
				else
				{
					ScheduleContainerType(Pair.Value.Trigger);
				}
			}
			Pair.Value.Value = Value;
		}
	}

	// Set action input values
	if (IsRunning(ActionState.Action.Storage))
	{
		UTGOR_InputModule* Module = ActionState.Action.Storage->GetModule<UTGOR_InputModule>();
		if (IsValid(Module))
		{
			Module->SetInput(Input, Value);
		}
	}
}
