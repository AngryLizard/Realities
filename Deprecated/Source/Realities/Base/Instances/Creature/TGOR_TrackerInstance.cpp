// The Gateway of Realities: Planes of Existence.


#include "TGOR_TrackerInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Mod/Trackers/TGOR_Tracker.h"


FTGOR_TrackerEntry::FTGOR_TrackerEntry()
	: Counter(0.0f)
{
}

void FTGOR_TrackerEntry::Merge(TArray<FTGOR_TrackerEntry>& Entries, const TArray<FTGOR_TrackerEntry>& Other, const FTGOR_TrackerEntry& Entry)
{
	// Merge counter
	Counter += Entry.Counter;

	// Merge sub counters
	for (const auto& Pair : Entry.Subs)
	{
		int32* Ptr = Subs.Find(Pair.Key);
		if (Ptr)
		{
			Entries[*Ptr].Merge(Entries, Other, Other[Pair.Value]);
		}
		else
		{
			const int32 Index = Entries.Emplace(FTGOR_TrackerEntry());
			Entries[Index].Merge(Entries, Other, Other[Pair.Value]);
		}
	}
}

void FTGOR_TrackerEntry::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Counter", Counter);
	Package.WriteEntry("Subs", Subs);
}

void FTGOR_TrackerEntry::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Counter", Counter);
	Package.ReadEntry("Subs", Subs);
}


float FTGOR_TrackerEntry::Count(const TArray<FTGOR_TrackerEntry>& Entries, const TArray<TSubclassOf<UTGOR_Content>>& Filter, int32 Count) const
{
	// Break recursion if all filter content has been found
	const int32 Num = Filter.Num();
	if (Count >= Num)
	{
		return Counter;
	}

	// Accumulate all valid children
	float Accumulate = 0.0;
	for (const TPair<UTGOR_Content*, int32> Pair : Subs)
	{
		// Add up number of matches recursively
		int32 Match = 0;
		for (const TSubclassOf<UTGOR_Content>& Class : Filter)
		{
			if (Pair.Key->IsA(Class))
			{
				Match = 1;
				break;
			}
		}

		// Add count to total
		const FTGOR_TrackerEntry& Entry = Entries[Pair.Value];
		Accumulate += Entry.Count(Entries, Filter, Count + Match);
	}
	return Accumulate;
}


void FTGOR_TrackerEntry::Put(TArray<FTGOR_TrackerEntry>& Entries, const TArray<UTGOR_Content*>& Registry, int32 Index, float Delta)
{
	if (Registry.IsValidIndex(Index))
	{
		UTGOR_Content* Content = Registry[Index];
		if (IsValid(Content))
		{
			Counter += Delta;
			int32* Sub = Subs.Find(Content);
			if (Sub)
			{
				// Put directly if already exists
				Entries[*Sub].Put(Entries, Registry, Index + 1, Delta);
			}
			else
			{
				// Create new entry otherwise
				FTGOR_TrackerEntry Entry;
				Subs.Emplace(Content, Entries.Num());
				Entries.Emplace(Entry);
				Entries.Last().Put(Entries, Registry, Index + 1, Delta);
			}
		}
		else
		{
			// Skip empty entries
			Put(Entries, Registry, Index + 1, Delta);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_TrackerInstance::FTGOR_TrackerInstance()
{
}

void FTGOR_TrackerInstance::Merge(const FTGOR_TrackerInstance& Other)
{
	Root.Merge(Entries, Other.Entries, Other.Root);
}

void FTGOR_TrackerInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Root", Root);
	Package.WriteEntry("Entries", Entries);
}

void FTGOR_TrackerInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Root", Root);
	Package.ReadEntry("Entries", Entries);
}

float FTGOR_TrackerInstance::Count(const TArray<TSubclassOf<UTGOR_Content>>& Filter) const
{
	return Root.Count(Entries, Filter, 0);
}

float FTGOR_TrackerInstance::Count() const
{
	return Root.Count(Entries, TArray<TSubclassOf<UTGOR_Content>>(), 0);
}

void FTGOR_TrackerInstance::Put(const TSet<UTGOR_Content*>& Registry, float Delta)
{
	TArray<UTGOR_Content*> List = Registry.Array();
	List.Remove(nullptr);
	List.Sort([](const UTGOR_Content& A, const UTGOR_Content& B) -> bool { return A.GetStorageIndex() < B.GetStorageIndex(); });
	Root.Put(Entries, List, 0, Delta);
}

void FTGOR_TrackerInstance::Put(float Delta)
{
	Root.Put(Entries, TArray<UTGOR_Content*>(), 0, Delta);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_TrackerData::FTGOR_TrackerData()
{
}

void FTGOR_TrackerData::Merge(const FTGOR_TrackerData& Other)
{
	// Merge trackers
	for (const auto& Pair : Other.Data)
	{
		FTGOR_TrackerInstance* Ptr = Data.Find(Pair.Key);
		if (Ptr)
		{
			Ptr->Merge(Pair.Value);
		}
		else
		{
			Data.Add(Pair.Key, Pair.Value);
		}
	}
}

void FTGOR_TrackerData::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Data", Data);
}

void FTGOR_TrackerData::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Data", Data);
}