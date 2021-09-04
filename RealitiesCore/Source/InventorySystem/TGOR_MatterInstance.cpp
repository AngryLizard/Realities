
#include "TGOR_MatterInstance.h"

#include "Content/TGOR_Segment.h"
#include "Content/TGOR_Matter.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MatterContainer)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MatterContainers)

void FTGOR_MatterContainer::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Segment", Segment);
	Package.WriteEntry("Slots", Slots);
}

void FTGOR_MatterContainer::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Segment", Segment);
	Package.ReadEntry("Slots", Slots);
}

void FTGOR_MatterContainer::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Segment);
	if (IsValid(Segment))
	{
		const FTGOR_Table Table = FTGOR_Table::Make(Segment->MatterRanks, Segment->MatterCapacity, Slots, 0);
		Package.WriteEntry(Table);
	}
}

void FTGOR_MatterContainer::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Segment);
	if (IsValid(Segment))
	{
		FTGOR_Table Table = FTGOR_Table::Make(Segment->MatterRanks, Segment->MatterCapacity, 0);
		Package.ReadEntry(Table);
		Table.Populate(Segment->MatterRanks, Slots);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

int32 FTGOR_MatterContainer::CountMatter(UTGOR_Matter* Matter) const
{
	const int32* Ptr = Slots.Find(Matter);
	if (Ptr)
	{
		return *Ptr;
	}
	return 0;
}

int32 FTGOR_MatterContainer::RemoveMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	if (Quantity > 0)
	{
		// Remove as much as we can and should
		if (int32* CompPtr = Slots.Find(Matter))
		{
			if (Quantity >= *CompPtr)
			{
				Quantity = *CompPtr;
				Slots.Remove(Matter);
			}
			else
			{
				Quantity = FMath::Min(Quantity, *CompPtr);
				*CompPtr -= Quantity;
			}
			return Quantity;
		}
	}
	return 0;
}

int32 FTGOR_MatterContainer::AddMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	if (Quantity > 0)
	{
		// Check whether Segment should have it to begin with
		if (const int32* CapacityPtr = Segment->MatterCapacity.Find(Matter))
		{
			if (int32* CompPtr = Slots.Find(Matter))
			{
				Quantity = (*CompPtr) + Quantity;
				*CompPtr = FMath::Min(Quantity, (*CapacityPtr));
				return Quantity - *CompPtr;
			}
			else
			{
				const int32 Result = FMath::Min(Quantity, (*CapacityPtr));
				Slots.Add(Matter, Result);
				return Quantity - Result;
			}
		}
	}
	return Quantity;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_MatterContainer::AddComposition(TMap<UTGOR_Matter*, int32>& Composition) const
{
	// Get full composition from Segment content
	for (const auto& Pair : Segment->MatterCapacity)
	{
		// See whether we are storing a value
		if (const int32* Ptr = Slots.Find(Pair.Key))
		{
			Composition.Add(Pair.Key, FMath::Min(*Ptr, Pair.Value));
		}
		else
		{
			Composition.Add(Pair.Key, 0);
		}
	}
}

void FTGOR_MatterContainer::AddMissing(TMap<UTGOR_Matter*, int32>& Missing) const
{
	// Get full composition from Segment content
	for (const auto& Pair : Segment->MatterCapacity)
	{
		// See whether we are storing a value
		if (const int32* Ptr = Slots.Find(Pair.Key))
		{
			Missing.Add(Pair.Key, FMath::Max(0, Pair.Value - (*Ptr)));
		}
		else
		{
			Missing.Add(Pair.Key, Pair.Value);
		}
	}
}

void FTGOR_MatterContainer::AddCapacity(TMap<UTGOR_Matter*, int32>& Capacity) const
{
	for (const auto& Pair : Segment->MatterCapacity)
	{
		Capacity.FindOrAdd(Pair.Key, 0) += Pair.Value;
	}
}

void FTGOR_MatterContainer::AddSupportedMatter(TSet<UTGOR_Matter*>& Supported) const
{
	for (const auto& Pair : Segment->MatterCapacity)
	{
		Supported.Emplace(Pair.Key);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void FTGOR_MatterContainers::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Containers", Containers);
}

void FTGOR_MatterContainers::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Containers", Containers);
}

void FTGOR_MatterContainers::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Containers);
}

void FTGOR_MatterContainers::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Containers);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 FTGOR_MatterContainers::CountMatter(TSubclassOf<UTGOR_Segment> Filter, UTGOR_Matter* Matter) const
{
	int32 Count = 0;
	for (const FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Container.CountMatter(Matter);
		}
	}
	return Count;
}

int32 FTGOR_MatterContainers::RemoveMatter(TSubclassOf<UTGOR_Segment> Filter, UTGOR_Matter* Matter, int32 Quantity)
{
	int32 Removed = 0;
	for (FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Removed += Container.RemoveMatter(Matter, Quantity - Removed);
		}
	}
	return Removed;
}

int32 FTGOR_MatterContainers::AddMatter(TSubclassOf<UTGOR_Segment> Filter, UTGOR_Matter* Matter, int32 Quantity)
{
	for (FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Quantity = Container.AddMatter(Matter, Quantity);
		}
	}
	return Quantity;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_MatterContainers::AddComposition(TSubclassOf<UTGOR_Segment> Filter, TMap<UTGOR_Matter*, int32>& Composition) const
{
	for (const FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Container.AddComposition(Composition);
		}
	}
}

void FTGOR_MatterContainers::AddMissing(TSubclassOf<UTGOR_Segment> Filter, TMap<UTGOR_Matter*, int32>& Missing) const
{
	for (const FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Container.AddMissing(Missing);
		}
	}
}

void FTGOR_MatterContainers::AddCapacity(TSubclassOf<UTGOR_Segment> Filter, TMap<UTGOR_Matter*, int32>& Capacity) const
{
	for (const FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Container.AddCapacity(Capacity);
		}
	}
}

void FTGOR_MatterContainers::AddSupported(TSubclassOf<UTGOR_Segment> Filter, TSet<UTGOR_Matter*>& Supported) const
{
	for (const FTGOR_MatterContainer& Container : Containers)
	{
		if (!*Filter || Container.Segment->IsA(Filter))
		{
			Container.AddSupportedMatter(Supported);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_MatterBlueprintLibrary::GetComposition(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter)
{
	TMap<UTGOR_Matter*, int32> Composition;
	Containers.AddComposition(Filter, Composition);
	return Composition;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterBlueprintLibrary::GetMissing(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter)
{
	TMap<UTGOR_Matter*, int32> Missing;
	Containers.AddMissing(Filter, Missing);
	return Missing;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterBlueprintLibrary::GetCapacity(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter)
{
	TMap<UTGOR_Matter*, int32> Capacity;
	Containers.AddCapacity(Filter, Capacity);
	return Capacity;
}

TSet<UTGOR_Matter*> UTGOR_MatterBlueprintLibrary::GetSupported(const FTGOR_MatterContainers& Containers, TSubclassOf<UTGOR_Segment> Filter)
{
	TSet<UTGOR_Matter*> Supported;
	Containers.AddSupported(Filter, Supported);
	return Supported;
}
