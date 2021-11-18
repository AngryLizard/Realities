// The Gateway of Realities: Planes of Existence.
#include "TGOR_PilotComponent.h"

#include "CoreSystem/TGOR_Singleton.h"

#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "DimensionSystem/Tasks/TGOR_PilotTask.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

#include "DimensionSystem/Content/TGOR_Primitive.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"

#include "DimensionSystem/Tasks/TGOR_LinearPilotTask.h"

#include "Engine/NetConnection.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UTGOR_PilotComponent::UTGOR_PilotComponent()
:	Super(),
	AdjustThreshold()
{
}

void UTGOR_PilotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Make sure we are parented to surrounding volume if we aren't parented to anything
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (!IsValid(PilotTask) && SurroundingVolume.IsValid())
	{
		UTGOR_MobilityComponent* Movement = SurroundingVolume->GetMovement();
		if (IsValid(Movement))
		{
			Movement->ParentLinear(this, INDEX_NONE, ComputeSpace());
		}

		// Can't parent to myself
		if (Movement != this)
		{
			// Check whether it worked
			PilotTask = GetPilotTask();
			if (!IsValid(PilotTask))
			{
				ERROR("Pilot object without task found", Warning);
			}
		}
	}
	SetComponentPosition(ComputePosition());
}

void UTGOR_PilotComponent::DestroyComponent(bool bPromoteChildren)
{
	for (UTGOR_PilotTask* PilotTask : PilotSlots)
	{
		PilotTask->Unparent();
	}
	PilotSlots.Empty();

	Super::DestroyComponent(bPromoteChildren);
}

void UTGOR_PilotComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_PilotComponent, PilotSlots, COND_None); // This MUST be replicated before setup
	DOREPLIFETIME_CONDITION(UTGOR_PilotComponent, PilotState, COND_None);
}

void UTGOR_PilotComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	Super::GetSubobjectsWithStableNamesForNetworking(Objs);

	/* for (UTGOR_PilotTask* PilotTask : PilotSlots)
	{
		if (IsValid(PilotTask) && PilotTask->IsNameStableForNetworking())
		{
			Objs.Add(PilotTask);
		}
	} */
}

bool UTGOR_PilotComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UTGOR_PilotTask* PilotTask : PilotSlots)
	{
		if (IsValid(PilotTask))
		{
			WroteSomething |= Channel->ReplicateSubobject(PilotTask, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}


TSet<UTGOR_CoreContent*> UTGOR_PilotComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> Set;
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (IsValid(PilotTask))
	{
		Set.Add(PilotTask->GetPilot());
	}
	return Set;
}

const FTGOR_MovementSpace UTGOR_PilotComponent::ComputeSpace() const
{
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (IsValid(PilotTask))
	{
		return PilotTask->ComputeSpace();
	}
	return Super::ComputeSpace();
}

const FTGOR_MovementPosition UTGOR_PilotComponent::ComputePosition() const
{
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (IsValid(PilotTask))
	{
		return PilotTask->ComputePosition();
	}
	return Super::ComputePosition();
}


bool UTGOR_PilotComponent::HasParent(const UTGOR_MobilityComponent* Component) const
{
	if(Super::HasParent(Component))
	{
		return true;
	}

	UTGOR_MobilityComponent* Parent = GetParent();
	if (Parent == Component)
	{
		return true;
	}
	return IsValid(Parent) ? Parent->HasParent(Component) : false;
}

void UTGOR_PilotComponent::OnPositionChange(const FTGOR_MovementPosition& Position)
{
	Super::OnPositionChange(Position);

	ATGOR_PhysicsVolume* Volume = QueryVolume(Position.Linear, SurroundingVolume.Get());
	if (Volume != SurroundingVolume)
	{
		// Notify owner
		OnVolumeChanged.Broadcast(SurroundingVolume.Get(), Volume);
		SurroundingVolume = Volume;

		// Notify owner once if we are left without volume
		if (!SurroundingVolume.IsValid())
		{
			OnOutOfLevel.Broadcast();
		}
	}
}

void UTGOR_PilotComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	if (Body.Volume < SMALL_NUMBER)
	{
		FVector Origin, Extend;
		AActor* Actor = GetOwner();
		Actor->GetActorBounds(true, Origin, Extend);
		Body.SetFromBox(FVector(0.5f), Extend * 2, 1.0f);
	}

	UTGOR_PilotTask* CurrentTask = GetPilotTask();
	TMap<UTGOR_Pilot*, TArray<UTGOR_PilotTask*>> Previous;

	// Remove slots but cache both instances and items in case the new loadout can use them
	for (int Slot = 0; Slot < PilotSlots.Num(); Slot++)
	{
		UTGOR_PilotTask* PilotSlot = PilotSlots[Slot];
		if (IsValid(PilotSlot))
		{
			PilotSlot->Unparent();
			Previous.FindOrAdd(PilotSlot->GetPilot()).Add(PilotSlot);
		}
	}
	PilotSlots.Empty();

	// Add static slots
	TArray<UTGOR_Pilot*> Pilots = Dependencies.Spawner->GetMListFromType<UTGOR_Pilot>(SpawnPilots);

	// Generate slots
	for (UTGOR_Pilot* Pilot : Pilots)
	{
		if (IsValid(Pilot))
		{
			UTGOR_PilotTask* PilotSlot = nullptr;

			TArray<UTGOR_PilotTask*>* Ptr = Previous.Find(Pilot);
			if (Ptr && Ptr->Num() > 0)
			{
				PilotSlot = Ptr->Pop();
			}
			else
			{
				// No cache was found, create a new one
				PilotSlot = Pilot->CreatePilotTask(this, PilotSlots.Num());
			}

			// Initialise and add to slots
			if (IsValid(PilotSlot))
			{
				PilotSlots.Add(PilotSlot);
			}
		}
	}

	// Reparent with current (or retry euclidean if task got removed)
	const int32 ActiveSlot = PilotSlots.Find(CurrentTask);
	if (PilotSlots.IsValidIndex(ActiveSlot))
	{
		AttachWith(ActiveSlot);
	}
	else
	{
		Teleport(ComputeSpace());
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_PilotTask* PilotSlot : Pair.Value)
		{
			PilotSlot->Unparent();
			PilotSlot->MarkPendingKill();
		}
	}

	const float Scale = GetComponentScale().GetMin();
	const float Strength = Scale * Scale * Scale;

	FTGOR_MovementBody NewBody;
	NewBody.SetFromCapsule(SpawnSurfaceArea, GetScaledCapsuleRadius(), GetScaledCapsuleHalfHeight(), SpawnWeight * Strength);
	SetBody(NewBody);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_PilotComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = PilotSlots.Num();
	for (UTGOR_PilotTask* PilotSlot : PilotSlots)
	{
		Modules.Emplace(PilotSlot->Identifier.Slot, PilotSlot->Identifier.Content);
	}
	return Modules;
}

bool UTGOR_PilotComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	if (IsValid(Dimension))
	{
		SurroundingVolume = Dimension->GetLevelVolume();
	}

	return ITGOR_DimensionInterface::PreAssemble(Dimension);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PilotComponent::SetNetSmooth(FVector Delta, FRotator Rotation)
{
	const FTGOR_MovementPosition Space = ComputeSpace();
	FTGOR_MovementPosition Position;
	NetSmoothDelta.Linear += Space.Linear + Delta;
	NetSmoothDelta.Angular = Space.Angular * Rotation.Quaternion();
}

void UTGOR_PilotComponent::NetSmooth(float Timestep, float Speed)
{
	NetSmoothDelta.Linear = FMath::VInterpTo(NetSmoothDelta.Linear, FVector::ZeroVector, Timestep, Speed);
	NetSmoothDelta.Angular = FMath::QInterpTo(NetSmoothDelta.Angular, FQuat::Identity, Timestep, Speed);
}

void UTGOR_PilotComponent::SetComponentFromSmoothDelta(USceneComponent* Component, const FTransform& InitialWorld)
{
	const FTransform Translate = FTransform(NetSmoothDelta.Linear);
	const FTransform Rotation = FTransform(NetSmoothDelta.Angular);
	Component->SetWorldTransform(Rotation * InitialWorld * Translate);
}

/*
void UTGOR_PilotComponent::MovementAdjust(const FTGOR_MovementBase& Old, const FTGOR_MovementBase& New)
{
	const FTGOR_MovementPosition OldSpace = Old.ComputeSpace();
	const FTGOR_MovementPosition NewSpace = New.ComputeSpace();
	NetSmoothDelta.Linear += OldSpace.Linear - NewSpace.Linear;
	NetSmoothDelta.Angular = (OldSpace.Angular * NewSpace.Angular.Inverse()) * NetSmoothDelta.Angular;

	// Reset if error is too high
	if (NetSmoothDelta.Compare(FTGOR_MovementPosition(), NetDeltaThreshold) > 1.0f)
	{
		NetSmoothDelta = FTGOR_MovementPosition();
	}
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_PilotComponent::Teleport(const FTGOR_MovementDynamic& Dynamic)
{
	ATGOR_PhysicsVolume* Volume = QueryVolume(Dynamic.Linear);
	if (IsValid(Volume)) // This only fails if a location outside any level volume has been given
	{
		UTGOR_PilotComponent* Movement = Volume->GetMovement();
		if (IsValid(Movement))
		{
			const FTGOR_MovementDynamic& Old = ComputeSpace();
			if (Movement->ParentLinear(this, INDEX_NONE, Dynamic))
			{
				OnTeleportedMovement.Broadcast(Old, Dynamic, Volume);
			}
		}
	}
	return false;
}

ATGOR_PhysicsVolume* UTGOR_PilotComponent::QueryVolume(const FVector& Location, ATGOR_PhysicsVolume* ReferenceVolume) const
{
	SINGLETON_RETCHK(nullptr);

	ATGOR_PhysicsVolume* Volume = nullptr;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		ATGOR_PhysicsVolume* MainVolume = WorldData->GetMainVolume();

		// Check current volume before querying everything
		if (IsValid(ReferenceVolume) && ReferenceVolume->IsInside(Location))
		{
			Volume = ReferenceVolume->Query(Location);
		}
		else if (IsValid(MainVolume))
		{
			Volume = MainVolume->Query(Location);

			// Find whether we're out of level
			if (Volume == MainVolume && !Volume->IsInside(Location))
			{
				return nullptr;
			}
		}

		// If in own gravity field, assume parent instead
		if (IsValid(Volume))
		{
			UTGOR_MobilityComponent* VolumeMovement = Volume->GetMovement();
			if (IsValid(VolumeMovement) && VolumeMovement->HasParent(this))
			{
				Volume = Volume->GetParentVolume();
			}
		}
	}
	return Volume;
}

FTGOR_MovementDynamic UTGOR_PilotComponent::ComputeBase() const
{
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (IsValid(PilotTask))
	{
		return PilotTask->ComputeBase();
	}
	return FTGOR_MovementDynamic();
}

UTGOR_PilotTask* UTGOR_PilotComponent::GetPilotTask() const
{
	if (PilotSlots.IsValidIndex(PilotState.ActiveSlot))
	{
		return PilotSlots[PilotState.ActiveSlot];
	}
	return nullptr;
}

UTGOR_MobilityComponent* UTGOR_PilotComponent::GetParent() const
{
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if(IsValid(PilotTask))
	{
		return PilotTask->GetParent();
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_PilotComponent::Detach()
{
	UTGOR_MobilityComponent* Parent = GetParent();
	if (Parent)
	{
		return Parent->ParentLinear(this, INDEX_NONE, ComputeSpace());
	}
	return false;
}

bool UTGOR_PilotComponent::IsAttachedWith(int32 Identifier) const
{
	return PilotState.ActiveSlot == Identifier;
}

void UTGOR_PilotComponent::AttachWith(int32 Identifier)
{
	if (Identifier != PilotState.ActiveSlot)
	{
		if (PilotSlots.IsValidIndex(PilotState.ActiveSlot))
		{
			PilotSlots[PilotState.ActiveSlot]->Unregister();
		}

		PilotState.ActiveSlot = Identifier;

		if (PilotSlots.IsValidIndex(PilotState.ActiveSlot))
		{
			PilotSlots[PilotState.ActiveSlot]->Register();
		}
		OnPilotChanged.Broadcast();
	}
}

UTGOR_PilotTask* UTGOR_PilotComponent::GetPilotOfType(TSubclassOf<UTGOR_PilotTask> Type) const
{
	if (*Type)
	{
		for (UTGOR_PilotTask* PilotSlot : PilotSlots)
		{
			if (PilotSlot->IsA(Type))
			{
				return PilotSlot;
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_PilotComponent::RepNotifyPilotState(const FTGOR_PilotState& Old)
{
	SetComponentPosition(ComputePosition());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_PilotComponent* UTGOR_PilotComponent::FindOwningPilot(USceneComponent* Component)
{
	if (!IsValid(Component))
	{
		return nullptr;
	}

	UTGOR_PilotComponent* Pilot = Cast<UTGOR_PilotComponent>(Component);
	if (IsValid(Pilot))
	{
		return Pilot;
	}

	return FindOwningPilot(Component->GetAttachParent());
}

UTGOR_PilotComponent* UTGOR_PilotComponent::FindRootPilot(AActor* Actor)
{
	return Cast<UTGOR_PilotComponent>(Actor->GetRootComponent());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementParent UTGOR_PilotComponent::FindReparentToActor(AActor* Actor, const FName& Name) const
{
	FTGOR_MovementParent Parent;
	if (IsValid(Actor))
	{
		// Only grab parentable components
		// TODO: Make interface to look for specific bones
		Parent.Mobility = Actor->FindComponentByClass<UTGOR_MobilityComponent>();
		if (IsValid(Parent.Mobility))
		{
			Parent.Index = Parent.Mobility->GetIndexFromName(Name);
			return Parent;
		}
	}

	if (SurroundingVolume.IsValid())
	{
		// Assume level volume if no contact was established
		Parent.Mobility = SurroundingVolume->GetMovement();
		Parent.Index = Parent.Mobility->GetIndexFromName(Name);
	}
	return Parent;
}

FTGOR_MovementParent UTGOR_PilotComponent::FindReparentToComponent(UActorComponent* Component, const FName& Name) const
{
	if (IsValid(Component))
	{
		return FindReparentToActor(Component->GetOwner(), Name);
	}
	else
	{
		return FindReparentToActor(nullptr, Name);
	}
}