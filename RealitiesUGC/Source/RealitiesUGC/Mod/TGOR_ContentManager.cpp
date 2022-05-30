// The Gateway of Realities: Planes of Existence.


#include "TGOR_ContentManager.h"
#include "RealitiesUGC/Gameplay/TGOR_ModGameInstance.h"
#include "RealitiesUGC/Mod/TGOR_ModRegistry.h"
#include "RealitiesUGC/Mod/TGOR_Content.h"
#include "RealitiesUGC/Mod/TGOR_ModSetup.h"
#include "RealitiesUGC/Mod/TGOR_Mod.h"

#if WITH_EDITOR
#include "Editor/UnrealEd/Public/Kismet2/KismetEditorUtilities.h"
#endif

#define LOCTEXT_NAMESPACE "TGOR_ContentManager"

UTGOR_ContentManager::UTGOR_ContentManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ContentRoot = ObjectInitializer.CreateDefaultSubobject<UTGOR_Content>(this, TEXT("ContentRoot"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ContentManager::ApplyModEntries(UTGOR_ModRegistry* Registry, UTGOR_Mod* Mod, const TArray<FTGOR_ContentLink>& Entries)
{
	if (!IsValid(Registry)) return;

	// Apply all modload entries
	for (const FTGOR_ContentLink& Entry : Entries)
	{
		// Operate on the hashtable
		switch (Entry.Action)
		{
			case ETGOR_ModActionEnumeration::Remove:
			{
				// Find content
				UTGOR_Content* Content = FindFirstOfType(Entry.Content);
				if (IsValid(Content))
				{
					ContentRoot->RemoveContent(Content);
				}
				break;
			}
			case ETGOR_ModActionEnumeration::Add:
			{
				// Create instance
				UTGOR_Content* Content = Registry->ConstructContent(Entry.Content);
				if (IsValid(Content))
				{
					Content->AssignWorld(GetWorld());
					Content->Mod = Mod;
					Content->BuildDefaultName(Mod->Entry.Name);
					Content->BuildResource();

					ContentRoot->AddContent(Content);
				}
				break;
			}
		}
	}
}

void UTGOR_ContentManager::ApplyModInsertions(const TArray<FTGOR_ContentInsertion>& Insertions)
{
	// Add insertions
	for (const FTGOR_ContentInsertion& Insertion : Insertions)
	{
		UTGOR_Content* Content = FindFirstOfType(Insertion.Target);
		if (IsValid(Content))
		{
			switch (Insertion.Action)
			{
				case ETGOR_ModActionEnumeration::Remove:
				{
					//Content->InsertionClasses.Remove(Insertion.Content);
					break;
				}
				case ETGOR_ModActionEnumeration::Add:
				{
					//Content->InsertionClasses.AddUnique(Insertion.Content);
					break;
				}
			}
		}
	}
}

void UTGOR_ContentManager::ApplyMod(UTGOR_ModRegistry* Registry, UTGOR_Mod* Mod, const TArray<FTGOR_ContentLink>& Assets)
{
	check(IsValid(Mod))

	// Apply filtertags
	FilterTags.AppendTags(Mod->FilterTags);

	// Initialise root
	ContentRoot->Mod = nullptr;
	ContentRoot->BuildDefaultName("BuiltIn");
	ContentRoot->BuildResource();

	// Compute mods to be added/removed/replaced
	ApplyModEntries(Registry, Mod, Mod->PreModLoad(ContentRoot));
	ApplyModEntries(Registry, Mod, Assets);
	ApplyModEntries(Registry, Mod, Mod->ContentLinks);
	ApplyModInsertions(Mod->ContentInsertions);

	Mod->PostModLoad(ContentRoot);
}

void UTGOR_ContentManager::ApplyInsertions()
{
	// Populate storage now that all content classes are loaded in
	ContentRoot->PopulateStorage(ContentStorage, ContentDictionary);
	
	// Update all insertions
	for (UTGOR_Content* Content : ContentStorage)
	{
		//ContentRoot->ConsiderInsertion(Content);
		ContentRoot->RecursiveMoveInsertion(Content, ETGOR_InsertionActionEnumeration::Add);
	}
}

void UTGOR_ContentManager::PostLoadContent()
{
	// Update all content classes
	ContentRoot->ForAllContent([](UTGOR_Content* Content)->bool
	{
		Content->PostBuildResource();
		return true;
	});
}

bool UTGOR_ContentManager::ValidateContent()
{
	bool IsValid = true;

	// We're only interested in non-abstract content
	for (UTGOR_Content* Content : ContentStorage)
	{
		if (!Content->Validate())
		{
			ERRET(FString::Printf(TEXT("Validation failed for %s"), *Content->GetName()), Warning, false);
			IsValid = false;
		}
	}

	return IsValid;
}

void UTGOR_ContentManager::ResetContent()
{
	ContentStorage.Reset();
	ContentDictionary.Reset();
	if(IsValid(ContentRoot))
	{
		ContentRoot->Reset();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Content* UTGOR_ContentManager::FindFirstOfType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		if(ContentRoot->IsA(Type))
		{
			return(ContentRoot);
		}
		return(ContentRoot->FindFirstOfType(Type));
	}
	return nullptr;
}

TArray<UTGOR_Content*> UTGOR_ContentManager::FindAllOfType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		if (ContentRoot->IsA(Type))
		{
			TArray<UTGOR_Content*> Out;
			Out.Emplace(ContentRoot);
			return Out;
		}
		return ContentRoot->FindAllOfType(Type);
	}
	return TArray<UTGOR_Content*>();
}

UTGOR_Content* UTGOR_ContentManager::GetContentFromType(TSubclassOf<UTGOR_Content> Type) const
{
	// Get content category
	UTGOR_Content* Content = FindFirstOfType(Type);
	if (IsValid(Content))
	{
		// Check whether any content exists
		if (Content->StorageSize > 0 &&
			ContentStorage.IsValidIndex(Content->StorageIndex))
		{
			// Grab from storage system
			return ContentStorage[Content->StorageIndex];
		}
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Info, nullptr);
	}
	return nullptr;
}

TArray<UTGOR_Content*> UTGOR_ContentManager::GetContentListFromType(TSubclassOf<UTGOR_Content> Type) const
{
	// Get content category
	TArray<UTGOR_Content*> List = FindAllOfType(Type);
	if (List.Num() > 0)
	{
		TArray<UTGOR_Content*> Out;
		for (UTGOR_Content* Content : List)
		{
			// Check whether any content exists
			if (Content->StorageSize > 0 &&
				ContentStorage.IsValidIndex(Content->StorageIndex) &&
				ContentStorage.IsValidIndex(Content->StorageIndex + Content->StorageSize - 1))
			{
				// Grab range from storage system
				UTGOR_Content* const* Data = ContentStorage.GetData() + Content->StorageIndex;
				Out.Append(Data, Content->StorageSize);
			}
		}
		return Out;
	}
	else if(*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Info, TArray<UTGOR_Content*>());
	}
	return TArray<UTGOR_Content*>();
}

UTGOR_Content* UTGOR_ContentManager::GetContentFromIndex(TSubclassOf<UTGOR_Content> Type, int32 Index) const
{
	if (Index == INDEX_NONE) return nullptr;

	// Get content category
	UTGOR_Content* Content = FindFirstOfType(Type);
	if (IsValid(Content))
	{
		if (Content->StorageSize > Index &&
			ContentStorage.IsValidIndex(Content->StorageIndex + Index))
		{
			// Grab from storage system
			return(ContentStorage[Content->StorageIndex + Index]);
		}
		else
		{
			ERRET("Index out of range", Error, nullptr);
		}
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Info, nullptr);
	}
	return nullptr;
}

int32 UTGOR_ContentManager::GetContentIndex(TSubclassOf<UTGOR_Content> Type, UTGOR_Content* Content) const
{
	if (!IsValid(Content)) return INDEX_NONE;

	// Get content category
	UTGOR_Content* Base = FindFirstOfType(Type);
	if (IsValid(Base))
	{
		if (Base->StorageIndex <= Content->StorageIndex && Content->StorageIndex < Base->StorageIndex + Base->StorageSize)
		{
			// Get relative index
			return(Content->StorageIndex - Base->StorageIndex);
		}
		else
		{
			ERRET("Index out of range", Error, INDEX_NONE);
		}
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Info, INDEX_NONE);
	}
	return INDEX_NONE;
}

int32 UTGOR_ContentManager::GetContentCapacity(TSubclassOf<UTGOR_Content> Type) const
{
	// Get content category
	UTGOR_Content* Base = FindFirstOfType(Type);
	if (IsValid(Base))
	{
		return Base->StorageSize;
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Info, INDEX_NONE);
	}
	return INDEX_NONE;
}

UTGOR_Content* UTGOR_ContentManager::GetContentFromName(TSubclassOf<UTGOR_Content> Type, const FString& Name) const
{
	// Look up in dictionary
	const int32* Index = ContentDictionary.Find(Name);
	if (Index && ContentStorage.IsValidIndex(*Index))
	{
		return ContentStorage[*Index];
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Content* UTGOR_ContentManager::GetContentRoot() const
{ 
	return ContentRoot; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ContentManager::ClearModSetups()
{
	Setups.Empty();
}

void UTGOR_ContentManager::AddModSetup(TSubclassOf<UTGOR_ModSetup> Type)
{
	if (*Type)
	{
		UTGOR_ModSetup* Setup = NewObject<UTGOR_ModSetup>(this, Type);
		if (*Setup->ParentSetup)
		{
			const int32 SetupNum = Setups.Num();
			for (int32 Index = 0; Index < SetupNum; Index++)
			{
				if (Setups[Index]->IsA(Setup->ParentSetup))
				{
					Setups.Insert(Setup, Index);
					return;
				}
			}
		}

		Setups.Emplace(Setup);
	}
}

bool UTGOR_ContentManager::ProgressSetups(bool IsServer)
{
	while (Setups.Num() > 0)
	{
		// Attempt next setup task, abort if we need to wait longer
		UTGOR_ModSetup* Setup = Setups[0];
		if (Setup->SetupOwner == ETGOR_SetupOwnerEnumeration::All ||
			(Setup->SetupOwner == ETGOR_SetupOwnerEnumeration::ClientOnly && !IsServer) ||
			(Setup->SetupOwner == ETGOR_SetupOwnerEnumeration::ServerOnly && IsServer))
		{
			const bool Success = Setup->Attempt(IsServer);
			if (!Success)
			{
				return true;
			}
		}
		Setups.RemoveAt(0);
		return true;
	}
	return false;
}

const TArray<UTGOR_ModSetup*>& UTGOR_ContentManager::GetSetups() const
{
	return Setups;
}

bool UTGOR_ContentManager::HasAnySetups() const
{
	return Setups.Num() > 0;
}

const TArray<UTGOR_Mod*>& UTGOR_ContentManager::GetMods() const
{
	return ModStorage;
}


TArray<FTGOR_ContentLink> UTGOR_ContentManager::Load(UTGOR_ModRegistry* Registry, UTGOR_Mod* Mod)
{
	// Load plugins
	if (IsValid(Registry))
	{
		// Load paths to assets
		TArray<UClass*> Classes;
		Registry->GetContentInMod(Mod, Classes);

		TArray<FTGOR_ContentLink> Links;
		for (UClass* Class : Classes)
		{
			if (Class->IsChildOf(UTGOR_Content::StaticClass()))
			{
				FTGOR_ContentLink Link;
				Link.Action = ETGOR_ModActionEnumeration::Add;
				Link.Content = Class;

				Links.Add(Link);
			}
		}

		return Links;
	}
	return TArray<FTGOR_ContentLink>();
}

TArray<FTGOR_ContentLink> UTGOR_ContentManager::LoadCore()
{
	TArray<FTGOR_ContentLink> Links;

	for (TObjectIterator< UClass > ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;

#if WITH_EDITOR
		if (FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
		{
			continue;
		}
#endif

		// Only interested in native C++ classes
		if (Class->IsNative() && Class->IsChildOf(UTGOR_Content::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			FTGOR_ContentLink Link;
			Link.Action = ETGOR_ModActionEnumeration::Add;
			Link.Content = Class;

			Links.Add(Link);
		}
	}
	return Links;
}

bool CheckDependency(const TArray<UTGOR_Mod*>& UsedMods, UTGOR_Mod* Mod)
{
	// Check whether all dependencies are present
	for (TSubclassOf<UTGOR_Mod> Dependency : Mod->Dependencies)
	{
		bool HasDependency = false;
		for (UTGOR_Mod* UsedMod : UsedMods)
		{
			if (UsedMod->IsA(Dependency))
			{
				HasDependency = true;
			}
		}

		if (!HasDependency)
		{
			return false;
		}
	}
	return true;
}

FTGOR_ModInstance UTGOR_ContentManager::GetLocalModSetup(UTGOR_ModRegistry* Registry, TSubclassOf<UTGOR_Mod> CoreModType)
{
	FTGOR_ModInstance ModSetup;

	//LoadPaths(UTGOR_Mod::StaticClass(), L"", true, Verbose, Classes);
	TMap<UClass*, FName> Classes;
	Registry->GetModsInPackages(Classes);

	// Load local mods
	TArray<UTGOR_Mod*> Mods;
	for (const auto& Pair : Classes)
	{
		UClass* Class = Pair.Key;
		if (Class->IsChildOf(UTGOR_Mod::StaticClass()))
		{
			UTGOR_Mod* Mod = Class->GetDefaultObject<UTGOR_Mod>();
			if (Mod == nullptr || Mod->Ignore) continue;

			Mods.Add(Mod);
		}
	}

	// Sort mods by dependencies
	Mods.Sort([](const UTGOR_Mod& A, const UTGOR_Mod& B) -> bool { return A.HasDependency(&B); });
	Algo::Reverse(Mods);

	// Make sure there is only one core
	bool HasCore = false;

	// Create entry list
	TArray<UTGOR_Mod*> UsedMods;
	ModSetup.ActiveMods.Reset();
	for (UTGOR_Mod* Mod : Mods)
	{
		// Make sure mod actually exists
		if (CheckDependency(UsedMods, Mod))
		{
			if (Mod->IsA<UTGOR_CoreMod>())
			{
				if (*CoreModType && Mod->IsA(CoreModType) && !HasCore)
				{
					ModSetup.ActiveMods.Add(Mod->Entry);
					UsedMods.Add(Mod);
					HasCore = true;
				}
			}
			else
			{
				UsedMods.Add(Mod);
				ModSetup.ActiveMods.Add(Mod->Entry);
			}
		}
	}
	return ModSetup;
}
#pragma optimize( "", off )
ETGOR_ModloadEnumeration UTGOR_ContentManager::LoadModSetup(UTGOR_ModRegistry* Registry, const FTGOR_ModInstance& ModSetup)
{
	ClearModSetups();
	ResetContent();

	//LoadPaths(UTGOR_Mod::StaticClass(), L"", true, false, Classes);
	TMap<UClass*, FName> Classes;
	Registry->GetModsInPackages(Classes);

	// Create mod classes
	TArray<UTGOR_Mod*> Mods;
	for (const auto& Pair : Classes)
	{
		UClass* Class = Pair.Key;
		if (Class->IsChildOf(UTGOR_Mod::StaticClass()))
		{
			UTGOR_Mod* Mod = NewObject<UTGOR_Mod>(this, Class);
			Mod->PackagePath = Pair.Value;
			Mods.Add(Mod);
		}
	}

	// Load server mods
	//Version = FTGOR_Version();
	ModStorage.Empty();
	for (int i = 0; i < ModSetup.ActiveMods.Num(); i++)
	{
		const FTGOR_ModEntry& Entry = ModSetup.ActiveMods[i];
		bool Found = false;

		// Find mod with the same name in modlist
		for (UTGOR_Mod* Mod : Mods)
		{
			if (Mod->Entry.Name.Equals(Entry.Name) && !Found)
			{
				if (Mod->Entry.Version != Entry.Version) return(ETGOR_ModloadEnumeration::WrongVersion);

				// Link mod
				//Version.AddIdentifier(Mod->Name);
				for (TSubclassOf<UTGOR_ModSetup> SetupType : Mod->Setups)
				{
					AddModSetup(SetupType);
				}

				TArray<FTGOR_ContentLink> Links = Load(Registry, Mod);

				if (Mod->IsA<UTGOR_CoreMod>())
				{
					Links.Append(LoadCore());
				}

				ApplyMod(Registry, Mod, Links);
				ModStorage.Add(Mod);
				Found = true;
			}
		}

		if (!Found)
		{
			return ETGOR_ModloadEnumeration::NotFound;
		}
	}

	// Apply insertions
	ApplyInsertions();

	// Call post loaders
	for (UTGOR_Mod* Mod : ModStorage)
	{
		Mod->PostAllModLoad(ContentRoot);
	}

	PostLoadContent();
	return ETGOR_ModloadEnumeration::Success;
}
#pragma optimize("", on)

#undef LOCTEXT_NAMESPACE