
#include "TGOR_MatterModule.h"


#include "Realities/Mod/Spawner/Items/TGOR_MatterItem.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_MatterModule::UTGOR_MatterModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Server;
}

bool UTGOR_MatterModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	// TODO: Implement proper comparison
	const UTGOR_MatterModule* OtherModule = static_cast<const UTGOR_MatterModule*>(Other);
	return StorageChangeList.Num() == OtherModule->StorageChangeList.Num();
}

bool UTGOR_MatterModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_MatterModule* OtherModule = static_cast<const UTGOR_MatterModule*>(Other);
	StorageChangeList = OtherModule->StorageChangeList;
	return true;
}


void UTGOR_MatterModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Storage", StorageChangeList);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_MatterModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Storage", StorageChangeList);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_MatterModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(StorageChangeList);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_MatterModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(StorageChangeList);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_MatterModule::Clear()
{
	Super::Clear();
	StorageChangeList.Reset();
}

FString UTGOR_MatterModule::Print() const
{
	FString Out = "";
	TMap<UTGOR_Matter*, int32> Composition = GetComposition();
	for (const auto& Pair : Composition)
	{
		Out += FString::Printf(TEXT("%s = %d, "), *Pair.Key->GetDefaultName(), Pair.Value);
	}
	return Out + ", " + Super::Print();
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterModule::GetComposition() const
{
	// Get full composition from item content
	TMap<UTGOR_Matter*, int32> Composition;
	for (const auto& Pair : StorageCapacity)
	{
		// See whether we are storing a value
		if (const int32* Ptr = StorageChangeList.Find(Pair.Key))
		{
			Composition.Add(Pair.Key, FMath::Min(*Ptr, Pair.Value));
		}
		else
		{
			Composition.Add(Pair.Key, 0);
		}
	}
	return Composition;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterModule::GetMissing() const
{
	// Get full composition from item content
	TMap<UTGOR_Matter*, int32> Missing;
	for (const auto& Pair : StorageCapacity)
	{
		// See whether we are storing a value
		if (const int32* Ptr = StorageChangeList.Find(Pair.Key))
		{
			Missing.Add(Pair.Key, FMath::Max(0, Pair.Value - (*Ptr)));
		}
		else
		{
			Missing.Add(Pair.Key, Pair.Value);
		}
	}
	return Missing;
}

int32 UTGOR_MatterModule::RemoveMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	if (Quantity > 0)
	{
		// Check whether item should have it to begin with
		if (const int32* ItemPtr = StorageCapacity.Find(Matter))
		{
			// Remove as much as we can and should
			if (int32* CompPtr = StorageChangeList.Find(Matter))
			{
				if (Quantity >= *CompPtr)
				{
					Quantity = *CompPtr;
					StorageChangeList.Remove(Matter);
				}
				else
				{
					Quantity = FMath::Min(Quantity, *CompPtr);
					*CompPtr -= Quantity;
				}
				MarkDirty();
				return Quantity;
			}
		}
	}
	return 0;
}

int32 UTGOR_MatterModule::AddMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	if (Quantity > 0)
	{
		// Check whether item should have it to begin with
		if (const int32* ItemPtr = StorageCapacity.Find(Matter))
		{
			if (int32* CompPtr = StorageChangeList.Find(Matter))
			{
				Quantity = (*CompPtr) + Quantity;
				*CompPtr = FMath::Min(Quantity, (*ItemPtr));
				MarkDirty();
				return Quantity - *CompPtr;
			}
			else
			{
				const int32 Result = FMath::Min(Quantity, (*ItemPtr));
				StorageChangeList.Add(Matter, Result);
				MarkDirty();
				return Quantity - Result;
			}
		}
	}
	return Quantity;
}

int32 UTGOR_MatterModule::CountMatter(UTGOR_Matter* Matter) const
{
	// Check whether item should have it to begin with and then whether we do
	const int32* ItemPtr = StorageCapacity.Find(Matter);
	const int32* CompPtr = StorageChangeList.Find(Matter);
	if (ItemPtr && CompPtr)
	{
		return *CompPtr;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


TSet<UTGOR_Matter*> UTGOR_MatterModule::GetSupportedMatter() const
{
	TSet<UTGOR_Matter*> Supported;
	for (const auto& Pair : StorageCapacity)
	{
		Supported.Emplace(Pair.Key);
	}
	return Supported;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterModule::PayStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition)
{
	TMap<UTGOR_Matter*, int32> Payment;
	for (const auto& Pair : Composition)
	{
		const int32 Quantity = RemoveMatter(Pair.Key, Pair.Value);
		Payment.FindOrAdd(Pair.Key) += Quantity;
	}
	return Payment;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterModule::InvestStorageMatter(const TMap<UTGOR_Matter*, int32>& Composition)
{
	TMap<UTGOR_Matter*, int32> Investment;
	for (const auto& Pair : Composition)
	{
		const int32 Quantity = AddMatter(Pair.Key, Pair.Value);
		Investment.FindOrAdd(Pair.Key) += Quantity;
	}
	return Investment;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MatterModule::AddStorageCapacity(UTGOR_Matter* Matter, int32 Capacity)
{
	StorageCapacity.Emplace(Matter, Capacity);
}

int32 UTGOR_MatterModule::GetStorageCapacity(UTGOR_Matter* Matter) const
{
	const int32* Ptr = StorageCapacity.Find(Matter);
	if (Ptr)
	{
		return *Ptr;
	}
	return 0;
}
