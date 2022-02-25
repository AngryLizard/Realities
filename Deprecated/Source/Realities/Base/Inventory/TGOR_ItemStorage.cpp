#include "TGOR_ItemStorage.h"

#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

void UTGOR_ItemStorage::Clear()
{
	Super::Clear();
	CompositionChangeList.Reset();
	MarkDirty();
}

FString UTGOR_ItemStorage::Print() const
{
	FString Out = "";
	TMap<UTGOR_Matter*, int32> Composition = GetComposition();
	for (const auto& Pair : Composition)
	{
		Out += FString::Printf(TEXT("%s = %d, "), *Pair.Key->GetDefaultName(), Pair.Value);
	}

	return FString::Printf(TEXT("Item = %s, Composition = {%s}, %s"), *ItemRef->GetDefaultName(), *Out, *Super::Print());
}

void UTGOR_ItemStorage::Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	if (IsValid(ItemRef))
	{
		ItemRef->Tick(Owner, this, DeltaSeconds);
	}
	Super::Tick(Owner, DeltaSeconds);
}

bool UTGOR_ItemStorage::Compatible(const UTGOR_Container* Other) const
{
	if (!Super::Compatible(Other))
	{
		return false;
	}

	const UTGOR_ItemStorage* OtherStorage = Cast<UTGOR_ItemStorage>(Other);
	return ItemRef == OtherStorage->ItemRef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_ItemStorage::GetMissing() const
{
	// Combine with module compositions
	TMap<UTGOR_Matter*, int32> Missing = Super::GetMissing();
	if (IsValid(ItemRef))
	{
		// Get missing quantities from item content
		for (const auto& Pair : ItemRef->CompositionCapacity)
		{
			// See whether we are storing a value
			if (const int32* Ptr = CompositionChangeList.Find(Pair.Key))
			{
				Missing.FindOrAdd(Pair.Key) += FMath::Min(*Ptr, Pair.Value);
			}
			else
			{
				Missing.FindOrAdd(Pair.Key) += 0;
			}
		}
	}

	return Missing;
}

TMap<UTGOR_Matter*, int32> UTGOR_ItemStorage::GetComposition() const
{
	// Combine with module compositions
	TMap<UTGOR_Matter*, int32> Composition = Super::GetComposition();
	if (IsValid(ItemRef))
	{
		// Get full composition from item content
		for (const auto& Pair : ItemRef->CompositionCapacity)
		{
			// See whether we are storing a value
			if (const int32* Ptr = CompositionChangeList.Find(Pair.Key))
			{
				Composition.FindOrAdd(Pair.Key) += FMath::Max(Pair.Value  - *Ptr, 0);
			}
			else
			{
				Composition.FindOrAdd(Pair.Key) += Pair.Value;
			}
		}
	}

	return Composition;
}

int32 UTGOR_ItemStorage::RemoveMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	// Remove from modules before own storage
	Quantity -= Super::RemoveMatter(Matter, Quantity);

	if (IsValid(ItemRef) && Quantity > 0)
	{
		// Check whether item should have it to begin with
		if (const int32* ItemPtr = ItemRef->CompositionCapacity.Find(Matter))
		{
			// Remove as much as we can and should
			if (int32* CompPtr = CompositionChangeList.Find(Matter))
			{
				Quantity = FMath::Min(Quantity, *ItemPtr - *CompPtr);
				*CompPtr += Quantity;
			}
			else
			{
				Quantity = FMath::Min(Quantity, *ItemPtr);
				CompositionChangeList.Add(Matter, Quantity);
			}

			MarkDirty();
			return Quantity;
		}
	}
	return 0;
}

int32 UTGOR_ItemStorage::AddMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	// Add to own storage before modules
	if (IsValid(ItemRef) && Quantity > 0)
	{
		// Check whether item should have it to begin with
		const int32* ItemPtr = ItemRef->CompositionCapacity.Find(Matter);
		int32* CompPtr = CompositionChangeList.Find(Matter);
		if (ItemPtr && CompPtr)
		{
			// Add as much as we can and should
			Quantity = (*CompPtr) - Quantity;
			if (Quantity < 0)
			{
				CompositionChangeList.Remove(Matter);
				MarkDirty();
				Quantity = -Quantity;
			}
			else
			{
				*CompPtr = Quantity;
				MarkDirty();
				Quantity = 0;
			}
		}
	}

	return Super::AddMatter(Matter, Quantity);
}

int32 UTGOR_ItemStorage::CountMatter(UTGOR_Matter* Matter) const
{
	// Count matter in modules if available
	int32 Count = Super::CountMatter(Matter);

	// Count intrinsic matter
	if (IsValid(ItemRef))
	{
		// Check whether item should have it to begin with
		if (const int32* ItemPtr = ItemRef->CompositionCapacity.Find(Matter))
		{
			Count += *ItemPtr;

			// Check whether we store it
			if (const int32* CompPtr = CompositionChangeList.Find(Matter))
			{
				Count -= *CompPtr;
			}
		}
	}
	return Count;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemStorage::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);
	Package.WriteEntry("Composition", CompositionChangeList);
}

bool UTGOR_ItemStorage::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);
	Package.ReadEntry("Composition", CompositionChangeList);
	return true;
}

void UTGOR_ItemStorage::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Send_Implementation(Package, Context);
	Package.WriteEntry(CompositionChangeList);
}

bool UTGOR_ItemStorage::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Recv_Implementation(Package, Context);
	Package.ReadEntry(CompositionChangeList);
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_ItemStorage::Equals(const UTGOR_Storage* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_ItemStorage* OtherStorage = Cast<UTGOR_ItemStorage>(Other);
	if (CompositionChangeList.Num() != OtherStorage->CompositionChangeList.Num()) return false;
	for (const auto& Pair : CompositionChangeList)
	{
		if (OtherStorage->CompositionChangeList[Pair.Key] != Pair.Value)
		{
			return false;
		}
	}
	return true;
}

UTGOR_Content* UTGOR_ItemStorage::GetOwner() const
{
	return GetItem();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Item* UTGOR_ItemStorage::GetItem() const
{
	return ItemRef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_ItemStorage::GetMass() const
{
	float Mass = 0.0f;

	// Count currently stored matter
	const TMap<UTGOR_Matter*, int32> Composition = GetComposition();
	for (const auto& Pair : Composition)
	{
		Mass += Pair.Value * Pair.Key->Mass;
	}

	return Mass;
}

bool UTGOR_ItemStorage::HasAnyMatter() const
{
	// Count currently stored matter
	const TMap<UTGOR_Matter*, int32> Composition = GetComposition();
	for (const auto& Pair : Composition)
	{
		if (Pair.Value > 0)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ItemStorage::EmptyMatter()
{
	// Set matter 
	if (IsValid(ItemRef))
	{
		CompositionChangeList = ItemRef->CompositionCapacity;
	}
}
