#include "TGOR_Storage.h"

#include "InventorySystem/Content/TGOR_Item.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"

#include "Modules/TGOR_Module.h"

void UTGOR_Storage::Clear()
{
	Super::Clear();
	for (auto& Pair : Modules)
	{
		Pair.Value->Clear();
	}
}

FString UTGOR_Storage::Print() const
{
	FString Out = "Modules = {";
	for (auto& Pair : Modules)
	{
		Out += FString::Printf(TEXT("%s = %s, "), *Pair.Key.ToString(), *Pair.Value->Print());
	}
	return Out + "}";
}

void UTGOR_Storage::Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	Super::Tick(Owner, DeltaSeconds);
	for (auto& Pair : Modules)
	{
		Pair.Value->Tick(Owner, DeltaSeconds);
	}
}

void UTGOR_Storage::Post()
{
	Super::Post();
	for (auto& Pair : Modules)
	{
		Pair.Value->Post();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_Storage::GetMissing() const
{
	TMap<UTGOR_Matter*, int32> Missing;

	// Combine with module missing quantities
	for (auto& Pair : Modules)
	{
		Pair.Value->CombineMissingOnto(Missing);
	}
	return Missing;
}

TMap<UTGOR_Matter*, int32> UTGOR_Storage::GetComposition() const
{
	TMap<UTGOR_Matter*, int32> Composition;

	// Combine with module compositions
	for (auto& Pair : Modules)
	{
		Pair.Value->CombineCompositionOnto(Composition);
	}
	return Composition;
}

int32 UTGOR_Storage::RemoveMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	// Remove from modules before own storage
	int32 Count = 0;
	for (auto& Pair : Modules)
	{
		Count += Pair.Value->RemoveMatter(Matter, Quantity - Count);
	}

	return Count;
}

int32 UTGOR_Storage::AddMatter(UTGOR_Matter* Matter, int32 Quantity)
{
	for (auto& Pair : Modules)
	{
		Quantity = Pair.Value->AddMatter(Matter, Quantity);
	}
	return Quantity;
}

int32 UTGOR_Storage::CountMatter(UTGOR_Matter* Matter) const
{
	// Count matter in modules if available
	int32 Count = 0;
	for (auto& Pair : Modules)
	{
		Count += Pair.Value->CountMatter(Matter);
	}

	return Count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Storage::Contains(const UTGOR_Storage* Other) const
{
	for (auto& Pair : Modules)
	{
		if (Pair.Value->Contains(Other))
		{
			return true;
		}
	}
	return false; // TODO: Other == this // Does a storage contain itself? For now I assume not so "Purge" will never return false even though "Contains" returned true.
}

bool UTGOR_Storage::Purge(const UTGOR_Storage* Other)
{
	bool AnyPurge = false;
	for (auto& Pair : Modules)
	{
		if (Pair.Value->Purge(Other))
		{
			AnyPurge = true;
		}
	}
	return AnyPurge;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Storage::PurgeDirt()
{
	// Always purge all modules to prevent unnecessary resends
	for (auto& Pair : Modules)
	{
		if (Pair.Value->PurgeDirt())
		{
			Dirty = ETGOR_DirtEnumeration::Dirty;
		}
	}

	if (Dirty > ETGOR_DirtEnumeration::None)
	{
		Dirty = ETGOR_DirtEnumeration::None;
		return true;
	}
	return false;
}

bool UTGOR_Storage::HasNotify()
{
	// Always purge all modules to prevent unnecessary resends
	for (auto& Pair : Modules)
	{
		if (Pair.Value->HasNotify())
		{
			return true;
		}
	}
	return Dirty == ETGOR_DirtEnumeration::Notify;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Storage::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	// TODO: Technically the order of entries in a map is *not* given to be consistent, but it is always constructed the same way and never changed afterwards so we *should* be good.
	for (const auto& Pair : Modules)
	{
		Package.WriteEntry(Pair.Key.ToString(), Pair.Value);
	}
}

bool UTGOR_Storage::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	for (auto& Pair : Modules)
	{
		Package.ReadEntry(Pair.Key.ToString(), Pair.Value);
	}
	return true;
}

void UTGOR_Storage::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	for (const auto& Pair : Modules)
	{
		if (Pair.Value->IsRelevant(ETGOR_NetvarEnumeration::Server))
		{
			Package.WriteEntry(Pair.Value);
		}
	}
}

bool UTGOR_Storage::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	for (auto& Pair : Modules)
	{
		if (Pair.Value->IsRelevant(ETGOR_NetvarEnumeration::Server))
		{
			Package.ReadEntry(Pair.Value);
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Storage::Equals(const UTGOR_Storage* Other) const
{
	if (!Compatible(Other)) return false;

	// Compare module data
	for (const auto& Pair : Modules)
	{
		UTGOR_Module* const* Ptr = Other->Modules.Find(Pair.Key);
		if (!Ptr || !(*Ptr)->Equals(Pair.Value))
		{
			return false;
		}
	}
	return true;
}

bool UTGOR_Storage::Merge(const UTGOR_Storage* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Compatible(Other)) return false;

	for (const auto& Pair : Modules)
	{
		if (Pair.Value->IsRelevant(Priority))
		{
			if (!Pair.Value->Merge(Other->Modules[Pair.Key], Priority))
			{
				return false;
			}
		}
	}
	return true;
}

UTGOR_Content* UTGOR_Storage::GetOwner() const
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Storage::BuildModules(const FTGOR_StorageDeclaration& Setup)
{
	for (const auto& Pair : Setup.Modules)
	{
		UTGOR_Module* Module = NewObject<UTGOR_Module>(this, Pair.Value);
		if (IsValid(Module))
		{
			Module->BuildModule(this);
			Modules.Emplace(Pair.Key, Module);
		}
	}
}

UTGOR_Module* UTGOR_Storage::GetModuleValidated(TSubclassOf<UTGOR_Module> Type, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	UTGOR_Module* Module = GetModule(Type);
	if (IsValid(Module))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return Module;
	}
	return nullptr;
}

UTGOR_Module* UTGOR_Storage::GetModule(TSubclassOf<UTGOR_Module> Type) const
{
	for (const auto& Pair : Modules)
	{
		if (Pair.Value->IsA(Type))
		{
			return Pair.Value;
		}
	}
	return nullptr;
}
