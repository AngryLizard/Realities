// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventoryComponent.h"

#include "Net/UnrealNetwork.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"


UTGOR_InventoryComponent::UTGOR_InventoryComponent()
	: Super(),
	MaxItemCount(16)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

}

void UTGOR_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTGOR_InventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


bool UTGOR_InventoryComponent::CanStoreItemAt(int32 Identifier, UTGOR_ItemStorage* Storage) const
{
	if (Super::CanStoreItemAt(Identifier, Storage))
	{
		return Identifier < MaxItemCount;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemStorage* UTGOR_InventoryComponent::PutItem(UTGOR_ItemStorage* Storage)
{
	if (IsValid(Storage))
	{
		// Look for empty container that supports given item
		for (int32 Slot = 0; Slot < MaxItemCount; Slot++)
		{
			// SwapItemStorage does validity checks for us
			UTGOR_ItemStorage* Current = GetItemStorage(Slot);
			if (!IsValid(Current) && CanStoreItemAt(Slot, Storage))
			{
				UTGOR_ItemStorage* Residual = SwapItemStorage(Slot, Storage);
				OnItemPut.Broadcast(Storage);
				return Residual;
			}
		}
	}
	return Storage;
}


UTGOR_ItemStorage* UTGOR_InventoryComponent::CreateItem(TSubclassOf<UTGOR_Item> Type)
{
	SINGLETON_RETCHK(nullptr);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Item* Item = ContentManager->GetTFromType(Type);
	if (IsValid(Item))
	{
		UTGOR_ItemStorage* Storage = Item->CreateStorage();
		if (IsValid(Storage))
		{
			return PutItem(Storage);
		}
	}
	return nullptr;
}


