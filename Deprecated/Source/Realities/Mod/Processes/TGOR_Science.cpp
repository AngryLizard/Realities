// The Gateway of Realities: Planes of Existence.

#include "TGOR_Science.h"
//#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Base/Inventory/TGOR_ProcessStorage.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Components/Inventory/TGOR_ProcessComponent.h"
#include "Realities/Components/Inventory/TGOR_StorageComponent.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"

#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"

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

	for (const auto& Pair : InputCapacity)
	{
		Module->AddStorageCapacity(Pair.Key, Pair.Value);
	}
}

void UTGOR_Science::PostBuildResource()
{
	Super::PostBuildResource();

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Generate composition
	InputCapacity.Reset();
	for (const auto& Pair : MatterInput)
	{
		UTGOR_Matter* Content = ContentManager->GetTFromType<UTGOR_Matter>(Pair.Key); // *this breaks if Matter isn't included in science.h, see header for more.
		if (IsValid(Content))
		{
			InputCapacity.Add(Content, Pair.Value);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Science::IsValidInput(UTGOR_ItemComponent* Container) const
{
	for (TSubclassOf<UTGOR_Item> InputItem : InputItems)
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
