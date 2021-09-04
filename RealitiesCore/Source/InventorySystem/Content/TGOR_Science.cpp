// The Gateway of Realities: Planes of Existence.

#include "TGOR_Science.h"
//#include "InventorySystem/Content/TGOR_Matter.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "InventorySystem/Storage/Modules/TGOR_MatterModule.h"
#include "InventorySystem/Storage/TGOR_ProcessStorage.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"
#include "InventorySystem/Components/TGOR_StorageComponent.h"
#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "InventorySystem/Components/TGOR_ProcessComponent.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_Science::UTGOR_Science()
	: Super()
{
	Modules.Modules.Emplace("Matter", UTGOR_MatterModule::StaticClass());
}

float UTGOR_Science::Process(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage, float Antimatter)
{
	Super::Process(Owner, Storage, Antimatter);
	UTGOR_StorageComponent* Sockets = Owner->GetOwnerComponent<UTGOR_StorageComponent>();
	return OnProcess(Sockets, Storage, Antimatter);
}

bool UTGOR_Science::CanProcess(UTGOR_ProcessComponent* Owner, UTGOR_ProcessStorage* Storage) const
{
	if (Super::CanProcess(Owner, Storage))
	{
		UTGOR_StorageComponent* Sockets = Owner->GetOwnerComponent<UTGOR_StorageComponent>();
		if (IsValid(Sockets))
		{
			return OnCanProcess(Sockets, Storage);
		}
	}
	return false;
}

void UTGOR_Science::BuildStorage(UTGOR_ProcessStorage* Storage)
{
	Super::BuildStorage(Storage);

	UTGOR_MatterModule* Module = Storage->GetModule<UTGOR_MatterModule>();

	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	for (const auto& Pair : Capacity)
	{
		Module->AddStorageCapacity(Pair.Key, Pair.Value);
	}
}

void UTGOR_Science::PostBuildResource()
{
	Super::PostBuildResource();

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Science::IsValidInput(UTGOR_ItemComponent* Container) const
{
	for (TSubclassOf<UTGOR_Item> InputItem : ItemInsertions)
	{
		UTGOR_ItemStorage* Storage = Container->PeekStorage();
		UTGOR_Item* Item = Storage->GetItem();
		if (Item->IsA(InputItem))
		{
			return true;
		}
	}
	return false;
}

TArray<UTGOR_ItemComponent*> UTGOR_Science::FilterInput(const TArray<UTGOR_ItemComponent*>& Containers) const
{
	TArray<UTGOR_ItemComponent*> UsableItems;
	for (UTGOR_ItemComponent* Container : Containers)
	{
		if (IsValidInput(Container))
		{
			UsableItems.Emplace(Container);
		}
	}
	return UsableItems;
}

const TMap<UTGOR_Matter*, int32>& UTGOR_Science::GetInputCapacity() const
{
	const TMap<UTGOR_Matter*, int32>& Capacity = Instanced_MatterInsertions.Collection;
	return Capacity;
}

void UTGOR_Science::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ItemInsertions);
	MOV_INSERTION(MatterInsertions);
}

