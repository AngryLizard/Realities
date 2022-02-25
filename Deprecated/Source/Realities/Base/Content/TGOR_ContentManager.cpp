// The Gateway of Realities: Planes of Existence.


#include "TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Mod/TGOR_Mod.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Engine/World.h"


UTGOR_ContentManager::UTGOR_ContentManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ContentRoot = ObjectInitializer.CreateDefaultSubobject<UTGOR_Content>(this, TEXT("ContentRoot"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ContentManager::ApplyModEntries(UTGOR_Mod* Mod, const TArray<FTGOR_ContentLink>& Entries)
{
	// Apply all modload entries
	for (const FTGOR_ContentLink& Entry : Entries)
	{
		// Operate on the hashtable
		switch (Entry.Action)
		{
			case ETGOR_ModAction::Remove:
			{
				// Find content
				UTGOR_Content* Content = FindFirstOfType(Entry.Content);
				if (IsValid(Content))
				{
					ContentRoot->RemoveContent(Content);
				}
				break;
			}
			case ETGOR_ModAction::Add:
			{
				// Create instance
				UWorld* World = GetWorld();
				UTGOR_GameInstance* Instance = World->GetGameInstance<UTGOR_GameInstance>();
				UTGOR_Content* Content = Instance->ConstructContent(Entry.Content);

				Content->Mod = Mod;
				Content->BuildDefaultName(Mod->Name);
				Content->BuildResource(GetWorld());

				ContentRoot->AddContent(Content);
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
				case ETGOR_ModAction::Remove:
				{
					Content->InsertionClasses.Remove(Insertion.Content);
					break;
				}
				case ETGOR_ModAction::Add:
				{
					Content->InsertionClasses.AddUnique(Insertion.Content);
					break;
				}
			}
		}
	}
}

void UTGOR_ContentManager::ApplyMod(UTGOR_Mod* Mod, const TArray<FTGOR_ContentLink>& Assets)
{
	check(IsValid(Mod))

	// Apply filtertags
	FilterTags.AppendTags(Mod->FilterTags);

	// Initialise root
	ContentRoot->Mod = nullptr;
	ContentRoot->BuildDefaultName("BuiltIn");
	ContentRoot->BuildResource(GetWorld());

	// Compute mods to be added/removed/replaced
	ApplyModEntries(Mod, Mod->PreModLoad(ContentRoot));
	ApplyModEntries(Mod, Assets);
	ApplyModEntries(Mod, Mod->ContentLinks);
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
		ContentRoot->ConsiderInsertion(Content);
	}
}

void UTGOR_ContentManager::PostLoad()
{
	// Update all content classes
	ContentRoot->ForAllContent([](UTGOR_Content* Content)->bool
	{
		Content->PostBuildResource();
		return(true);
	});
}

void UTGOR_ContentManager::Reset()
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
	return(nullptr);
}

TArray<UTGOR_Content*> UTGOR_ContentManager::FindAllOfType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		if (ContentRoot->IsA(Type))
		{
			TArray<UTGOR_Content*> Out;
			Out.Emplace(ContentRoot);
			return(Out);
		}
		return(ContentRoot->FindAllOfType(Type));
	}
	return(TArray<UTGOR_Content*>());
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
			return(ContentStorage[Content->StorageIndex]);
		}
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Error, nullptr)
	}
	return(nullptr);
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
		return(Out);
	}
	else if(*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Error, TArray<UTGOR_Content*>())
	}
	return(TArray<UTGOR_Content*>());
}

UTGOR_Content* UTGOR_ContentManager::GetContentFromIndex(TSubclassOf<UTGOR_Content> Type, int32 Index) const
{
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
			ERRET("Index out of range", Error, nullptr)
		}
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Error, nullptr)
	}
	return(nullptr);
}

int32 UTGOR_ContentManager::GetContentIndex(TSubclassOf<UTGOR_Content> Type, UTGOR_Content* Content) const
{
	// Get content category
	UTGOR_Content* Base = FindFirstOfType(Type);
	if (IsValid(Base) && IsValid(Content))
	{
		if (Base->StorageIndex <= Content->StorageIndex && Content->StorageIndex < Base->StorageIndex + Base->StorageSize)
		{
			// Get relative index
			return(Content->StorageIndex - Base->StorageIndex);
		}
		else
		{
			ERRET("Index out of range", Error, -1)
		}
	}
	else if (*Type)
	{
		ERRET(Type->GetName() + " not found in content system", Error, -1)
	}
	return(-1);
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
		ERRET(Type->GetName() + " not found in content system", Error, -1)
	}
	return(-1);
}

UTGOR_Content* UTGOR_ContentManager::GetContentFromName(TSubclassOf<UTGOR_Content> Type, const FString& Name) const
{
	// Look up in dictionary
	const int32* Index = ContentDictionary.Find(Name);
	if (Index && ContentStorage.IsValidIndex(*Index))
	{
		return(ContentStorage[*Index]);
	}
	return(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Content* UTGOR_ContentManager::GetContentRoot() const
{ 
	return ContentRoot; 
}
