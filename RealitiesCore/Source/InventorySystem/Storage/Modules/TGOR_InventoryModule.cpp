
#include "TGOR_InventoryModule.h"

#include "InventorySystem/Components/TGOR_ContainerComponent.h"
#include "InventorySystem/TGOR_ItemInstance.h"

#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Content/TGOR_Item.h"

#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_InventoryModule::UTGOR_InventoryModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Server;
}

bool UTGOR_InventoryModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	// Compare sizes
	const UTGOR_InventoryModule* OtherModule = static_cast<const UTGOR_InventoryModule*>(Other);
	if (GetCurrentSize() != OtherModule->GetCurrentSize()) return false;

	// Inventories are ordered, so only need to compare items at the same index
	for (int i = 0; i < Items.Num(); i++)
	{
		const FTGOR_ItemInstance& Ours = Items[i];
		const FTGOR_ItemInstance& Theirs = OtherModule->Items[i];
		if (!Ours.Storage->Equals(Theirs.Storage))
		{
			return false;
		}
	}
	return true;
}

bool UTGOR_InventoryModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Compare sizes and just replace if different
	const UTGOR_InventoryModule* OtherModule = static_cast<const UTGOR_InventoryModule*>(Other);
	if (GetCurrentSize() != OtherModule->GetCurrentSize())
	{
		Items = OtherModule->Items;
		return true;
	}

	// Merge each item individually
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (!Items[Index].Storage->Compatible(OtherModule->Items[Index].Storage))
		{
			Items[Index].Storage = OtherModule->Items[Index].Storage;
		}
		else if(!Items[Index].Storage->Merge(OtherModule->Items[Index].Storage, Priority))
		{
			return false;
		}
	}
	return true;
}


void UTGOR_InventoryModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Items", Items);
	return Super::Write_Implementation(Package, Context);
}

bool UTGOR_InventoryModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Items", Items);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_InventoryModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Items);
	return Super::Send_Implementation(Package, Context);
}

bool UTGOR_InventoryModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Items);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_InventoryModule::Clear()
{
	Super::Clear();
	Items.Empty();
}

FString UTGOR_InventoryModule::Print() const
{
	FString Out = "";
	for (const FTGOR_ItemInstance& Instance : Items)
	{
		if (IsValid(Instance.Storage))
		{
			Out += FString::Printf(TEXT("{%s }, "), *Instance.Storage->Print());
		}
	}
	return Out + ", " + Super::Print();
}

void UTGOR_InventoryModule::Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	// Iterate backwards since item in current iteration could be removed during tick
	for (int32 Index = Items.Num() - 1; Index >= 0; Index--)
	{
		UTGOR_ItemStorage* Storage = Items[Index].Storage;
		if (IsValid(Storage))
		{
			Storage->Tick(Owner, DeltaSeconds);

			// If child changed, we changed.
			// TODO: Implement diffing for inventories as well
			if (Storage->PurgeDirt())
			{
				MarkDirty();
			}
		}
	}

	Super::Tick(Owner, DeltaSeconds);
}


TMap<UTGOR_Matter*, int32> UTGOR_InventoryModule::GetComposition() const
{
	// Combine all item compositions
	TMap<UTGOR_Matter*, int32> Composition;
	for (const FTGOR_ItemInstance& Instance : Items)
	{
		Instance.Storage->CombineCompositionOnto(Composition);
	}
	return Composition;
}

int32 UTGOR_InventoryModule::RemoveMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	// Remove matter in order, remove items that are drained
	int32 Count = 0;
	for (auto It = Items.CreateIterator(); It; It++)
	{
		Count += It->Storage->RemoveMatter(Matter, Quantity - Count);
		if (!It->Storage->HasAnyMatter())
		{
			It.RemoveCurrent();
			MarkDirty();
		}
	}
	return Count;
}

int32 UTGOR_InventoryModule::AddMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	for (const FTGOR_ItemInstance& Instance : Items)
	{
		Quantity = Instance.Storage->AddMatter(Matter, Quantity);
	}
	return Quantity;
}

int32 UTGOR_InventoryModule::CountMatter(UTGOR_Matter* Matter) const
{
	int32 Count = 0;
	for (const FTGOR_ItemInstance& Instance : Items)
	{
		Count += Instance.Storage->CountMatter(Matter);
	}
	return Count;
}


bool UTGOR_InventoryModule::Contains(const UTGOR_Storage* Other) const
{
	for (const FTGOR_ItemInstance& Instance : Items)
	{
		if (Instance.Storage->Contains(Other))
		{
			return true;
		}
	}
	return false;
}

bool UTGOR_InventoryModule::Purge(const UTGOR_Storage* Other)
{
	bool AnyPurges = false;
	for (auto It = Items.CreateIterator(); It; It++)
	{
		if ((*It).Storage == Other)
		{
			It.RemoveCurrent();
			MarkDirty();
			AnyPurges = true;
		}
		else if ((*It).Storage->Purge(Other))
		{
			AnyPurges = true;
		}
	}
	return AnyPurges;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_InventoryModule::AddItem(UTGOR_ItemStorage* Storage)
{
	if (IsSuitable(Storage))
	{
		Items.Emplace(Storage);
		// TODO: So far most item operations are done on one single item. Sorting every AddItem might become expensive in the future if more items are shuffled at once.
		Items.Sort();
		MarkDirty();
	}
}

bool UTGOR_InventoryModule::RemoveItem(UTGOR_ItemStorage* Storage)
{
	int32 Index = INDEX_NONE;
	if (!Items.Find(Storage, Index)) return false;

	return RemoveAt(Index);
}

UTGOR_ItemStorage* UTGOR_InventoryModule::GetItemAt(int32 Index) const
{
	if (IsIndexValid(Index))
	{
		return Items[Index].Storage;
	}

	return nullptr;
}

bool UTGOR_InventoryModule::IsSuitable(UTGOR_ItemStorage* Storage, bool IgnoreSize) const
{
	if (Contains(Storage)) // Cycle prevention
	{
		return false;
	}

	// Check boundaries
	if (!IgnoreSize && GetCurrentSize() + 1 > GetMaxSize())
	{
		return false;
	}

	if (IsValid(Storage))
	{
		UTGOR_Item* Item = Storage->GetItem();
		if (IsValid(Item))
		{
			if (Item->IsSupportedBy(StorageRestrictions))
			{
				return true;
			}
		}
	}

	return false;
}

int32 UTGOR_InventoryModule::GetItemIndex(UTGOR_ItemStorage* Item) const
{
	const int32 Num = Items.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		if (Items[Index].Storage == Item)
		{
			return Index;
		}
	}
	return INDEX_NONE;
}


bool UTGOR_InventoryModule::RemoveAt(int32 Index)
{
	if (IsIndexValid(Index))
	{
		Items.RemoveAt(Index);
		MarkDirty();
		return true;
	}

	return false;
}

int32 UTGOR_InventoryModule::GetMaxSize() const
{
	return StorageCapacity;
}

int32 UTGOR_InventoryModule::GetCurrentSize() const
{
	return Items.Num();
}

bool UTGOR_InventoryModule::IsIndexValid(int32 Index) const
{
	return Index >= 0 && Index < GetCurrentSize() && Items.IsValidIndex(Index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_InventoryModule::SetRestrictions(const FTGOR_ItemRestriction& Restrictions, int32 Capacity)
{
	StorageRestrictions = Restrictions;
	StorageCapacity = Capacity;
}