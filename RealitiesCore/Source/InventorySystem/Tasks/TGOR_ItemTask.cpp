// The Gateway of Realities: Planes of Existence.

#include "TGOR_ItemTask.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "InventorySystem/Components/TGOR_DropComponent.h"
#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "InventorySystem/Content/TGOR_Item.h"

UTGOR_ItemTask::UTGOR_ItemTask()
	: Super()
{
}

void UTGOR_ItemTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ItemTask, Identifier, COND_InitialOnly);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Item* UTGOR_ItemTask::GetItem() const
{
	return Identifier.Content;
}


UTGOR_ItemComponent* UTGOR_ItemTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_ItemTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

bool UTGOR_ItemTask::IsSupportedBy(const FTGOR_ItemRestriction& Restriction) const
{
	if (*Restriction.Item && Identifier.Content->IsA(Restriction.Item))
	{
		if (*Restriction.Task && IsA(Restriction.Task))
		{
			return true;
		}
	}
	return false;
}

ATGOR_ItemDropActor* UTGOR_ItemTask::CreateItemDrop()
{
	if (IsValid(Identifier.Component) && IsValid(Identifier.Content))
	{
		ETGOR_FetchEnumeration State;
		UTGOR_DimensionData* Dimension = Identifier.Component->GetDimension(State);
		if (IsValid(Dimension))
		{
			const int32 ActorIdewntifier = Dimension->GetUniqueActorIdentifier();
			const FVector Location = Identifier.Component->GetComponentLocation();
			const FRotator Rotation = Identifier.Component->GetComponentRotation();

			UTGOR_IdentityComponent* Identity = Dimension->AddDimensionObject(ActorIdewntifier, Identifier.Content, Location, Rotation, State);
			if (State == ETGOR_FetchEnumeration::Found)
			{
				ATGOR_ItemDropActor* DropActor = Cast<ATGOR_ItemDropActor>(Identity->GetOwner());
				if (IsValid(DropActor))
				{
					UTGOR_DropComponent* Drop = DropActor->GetDrop();
					Drop->FreeItem(Drop->PushItem(this));
					return DropActor;
				}
			}
		}
	}
	return nullptr;
}

void UTGOR_ItemTask::FreeToOwner()
{
	Identifier.Component->OnItemFreed.Broadcast(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ItemTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);
	OnInitialise();
	return true;
}

void UTGOR_ItemTask::RemovedFromInventory()
{
	OnRemovedFromInventory();
}

void UTGOR_ItemTask::AddedToInventory()
{
	OnAddedToInventory();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemTask::AssignInventory(UTGOR_InventoryComponent* Inventory)
{
	if (InventoryComponent.IsValid())
	{
		RemovedFromInventory();
	}

	InventoryComponent = Inventory;

	if (InventoryComponent.IsValid())
	{
		AddedToInventory();
	}
}

bool UTGOR_ItemTask::HasInventory() const
{
	return InventoryComponent.IsValid();
}