// The Gateway of Realities: Planes of Existence.


#include "TGOR_DisassembleQuery.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Inventory/TGOR_MatterComponent.h"
#include "Realities/Components/Inventory/TGOR_StorageComponent.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_DisassembleQuery::UTGOR_DisassembleQuery()
	: Super(),
	StorageComponent(nullptr)
{
}

TArray<UTGOR_Content*> UTGOR_DisassembleQuery::QueryContent() const
{
	TArray<UTGOR_Item*> Recipes;
	const int32 Count = Indices.Num();
	for (int i = 0; i < Count; i++)
	{
		Recipes.Emplace(GetRecipe(i));
	}
	return MigrateContentArray(Recipes);
}

void UTGOR_DisassembleQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, UTGOR_MatterComponent* MatterComponent, bool Completed)
{
	Super::AssignComponent(ActionComponent, MatterComponent, Completed);

	// Clear state
	Indices.Reset();

	// Get inventory of provided ActionComponent
	if (IsValid(MatterComponent))
	{
		StorageComponent = MatterComponent->GetOwnerComponent<UTGOR_StorageComponent>();
	}

	if(IsValid(StorageComponent) && IsValid(MatterComponent))
	{
		// Get currently available matter
		TSet<UTGOR_Matter*> Matter = HostComponent->GetSupportedMatter();

		// Add inventory slots
		TArray<UTGOR_ItemComponent*> Containers = StorageComponent->GetAttachedContainers();

		InventoryComponent->ForEachSlot([&](const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Item) -> bool
			{
				UTGOR_Item* Content = Item->GetItem();
				if (Content->CanDisassembleInto(Matter))
				{
					if (Completed)
					{
						Indices.Emplace(Index);
					}
				}
				else
				{
					if (!Completed)
					{
						Indices.Emplace(Index);
					}
				}
				return true;
			});
	}
}

UTGOR_Item* UTGOR_DisassembleQuery::GetRecipe(int32 Index) const
{
	if (IsValid(InventoryComponent) && Indices.IsValidIndex(Index))
	{
		UTGOR_ItemStorage* Storage = InventoryComponent->GetItemStorage(Indices[Index]);
		if (IsValid(Storage))
		{
			return Storage->GetItem();
		}
	}
	return nullptr;
}
