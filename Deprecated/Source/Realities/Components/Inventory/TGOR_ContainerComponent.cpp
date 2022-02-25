// The Gateway of Realities: Planes of Existence.

#include "TGOR_ContainerComponent.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Actors/Inventory/TGOR_ItemDropActor.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"

#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


UTGOR_ContainerComponent::UTGOR_ContainerComponent()
	: Super(),
	ItemDropImpulse(750.0f),
	ItemDropUpwards(0.05f),
	ItemDropScatter(0.2f)
{
}


TSet<UTGOR_Content*> UTGOR_ContainerComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ActiveContent;

	// TODO: Do we want to track all items? The possibilities are pretty nice but need to test what this does to the tracker system

	return ActiveContent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ContainerComponent::ContainerUpdate()
{
	OnSlotUpdate.Broadcast();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_ItemDropActor* UTGOR_ContainerComponent::DropItemStorage(UTGOR_ItemStorage* Storage)
{
	if (!IsValid(Storage))
	{
		return nullptr;
	}

	ETGOR_FetchEnumeration State;
	UTGOR_DimensionData* Dimension = GetDimension(State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		// Spawn item in current dimension

		AActor* OwnerActor = GetOwner();
		const FTransform Transform = OwnerActor->GetActorTransform();
		const int32 Identifier = Dimension->GetUniqueActorIdentifier();
		UTGOR_IdentityComponent* Identitiy = Dimension->AddDimensionObject(Identifier, Storage->GetItem(), Transform.GetLocation(), Transform.GetRotation().Rotator(), State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			ATGOR_ItemDropActor* DropActor = Cast<ATGOR_ItemDropActor>(Identitiy->GetOwner());

			// Frow item on drop
			UTGOR_MobilityComponent* OwnerMobilityComponent = GetOwnerComponent<UTGOR_MobilityComponent>();
			if (IsValid(OwnerMobilityComponent))
			{
				FTGOR_MovementSpace ParentSpace = OwnerMobilityComponent->ComputeSpace();
				const FVector Impulse = ParentSpace.Angular * (FVector(1.0f, 0.0f, 1.0f + ItemDropUpwards) + FMath::VRand() * ItemDropScatter) * ItemDropImpulse;

				UTGOR_ItemStorage* Residual = DropActor->AssignItem(Storage, ParentSpace, Impulse);
				DestroyItemStorage(Residual);
			}
			else
			{
				FTGOR_MovementSpace ParentSpace;
				ParentSpace.Linear = OwnerActor->GetActorLocation();
				ParentSpace.Angular = OwnerActor->GetActorQuat();

				UTGOR_ItemStorage* Residual = DropActor->AssignItem(Storage, ParentSpace, FVector::ZeroVector);
				DestroyItemStorage(Residual);
			}
			
			OnItemDrop.Broadcast(Storage);

			return DropActor;
		}
	}
	return nullptr;
}

void UTGOR_ContainerComponent::DestroyItemStorage(UTGOR_ItemStorage* Storage)
{
	if (!IsValid(Storage))
	{
		return;
	}
	// TODO: Maybe flag for destruction, but GC should take care of this
	OnItemDestruction.Broadcast(Storage);
}

bool UTGOR_ContainerComponent::PurgeItemStorage(UTGOR_ItemStorage* Storage)
{
	return false;
}