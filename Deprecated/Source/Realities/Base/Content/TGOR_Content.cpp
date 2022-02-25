// The Gateway of Realities: Planes of Existence.


#include "TGOR_Content.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

FTGOR_Display::FTGOR_Display()
:	Icon(nullptr),
	Aspect(1.0f),
	CustomColor(FLinearColor::Black),
	UseCustomColor(false)
{
}

UTGOR_Content::UTGOR_Content()
:	Super(),
	StorageSize(0),
	StorageIndex(-1),
	Mod(nullptr),
	Abstract(true),
	DefaultMod(""),
	DefaultName(L"")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Content::AddContent(UTGOR_Content* Content)
{
	// TODO: Make more efficient
	// No need to add something that is already here
	if (SubContent.Contains(Content))
	{
		return false;
	}

	// Integrate matching children into new content
	for (int32 i = SubContent.Num()-1; i >= 0; i--)
	{
		UTGOR_Content* Sub = SubContent[i];

		if (Sub->IsA(Content->GetClass()))
		{
			Content->AddContent(Sub);
			SubContent.RemoveAt(i);
			StorageSize -= Sub->StorageSize;
		}
	}

	// Integrate into child if matching
	for (UTGOR_Content* Sub : SubContent)
	{
		if (Content->IsA(Sub->GetClass()))
		{
			if (Sub->AddContent(Content))
			{
				StorageSize += Content->StorageSize;
				return true;
			}
			return false;
		}
	}

	// Append if no match was found
	SubContent.Emplace(Content);
	StorageSize += Content->StorageSize;
	return true;
}

bool UTGOR_Content::RemoveContent(UTGOR_Content* Content)
{
	// Remove from children
	for (int32 i = SubContent.Num() - 1; i >= 0; i--)
	{
		UTGOR_Content* Sub = SubContent[i];
		if (Sub == Content)
		{
			// Remove content itself
			SubContent.RemoveAt(i);
			StorageSize -= Sub->StorageSize;
			return true;
		}
		else if (Content->IsA(Sub->GetClass()))
		{
			// Remove from fitting branch
			if (Sub->RemoveContent(Content))
			{
				StorageSize -= Content->StorageSize;
				return true;
			}
			return false;
		}
	}

	return false;
}


void UTGOR_Content::PopulateStorage(TArray<UTGOR_Content*>& ContentStorage, TMap<FString, int32>& ContentDictionary)
{
	// Abstract content shares their index with their first non-abstract one
	StorageIndex = ContentStorage.Num();

	// Only abstract classes get added to storage
	if (!Abstract)
	{
		// Add to content storage
		ContentStorage.Emplace(this);

		// Add to dictionary
		ContentDictionary.Add(DefaultName, StorageIndex);
	}

	// Populate children
	for (UTGOR_Content* Sub : SubContent)
	{
		Sub->PopulateStorage(ContentStorage, ContentDictionary);
	}
}

UTGOR_Content* UTGOR_Content::FindFirstOfType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		for (UTGOR_Content* Sub : SubContent)
		{
			if (Sub->IsA(Type))
			{
				return(Sub);
			}
			else if (Type->IsChildOf(Sub->GetClass()))
			{
				// Only need to go down path that fits search
				return Sub->FindFirstOfType(Type);
			}
		}
	}
	return nullptr;
}

TArray<UTGOR_Content*> UTGOR_Content::FindAllOfType(TSubclassOf<UTGOR_Content> Type) const
{
	TArray<UTGOR_Content*> Out;
	if (*Type)
	{
		for (UTGOR_Content* Sub : SubContent)
		{
			if (Sub->IsA(Type))
			{
				Out.Emplace(Sub);
			}
			else if (Type->IsChildOf(Sub->GetClass()))
			{
				// Only need to go down path that fits search
				Out.Append(Sub->FindAllOfType(Type));
			}
		}
	}
	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_Content::ConsiderInsertion(UTGOR_Content* Content)
{
	// Check whether this content matches insertion list
	for (TSubclassOf<UTGOR_Content> ContentClass : InsertionClasses)
	{
		if (*ContentClass && Content->IsA(ContentClass))
		{
			// Add to all children
			AddInsertion(Content);
			return;
		}
	}

	// Check all subcontent
	for (UTGOR_Content* Sub : SubContent)
	{
		Sub->ConsiderInsertion(Content);
	}
}

void UTGOR_Content::AddInsertion(UTGOR_Content* Content)
{
	Insertions.AddUnique(Content);
	for (UTGOR_Content* Sub : SubContent)
	{
		Sub->AddInsertion(Content);
	}
}

void UTGOR_Content::RemoveInsertion(UTGOR_Content* Content)
{
	Insertions.Remove(Content);
	for (UTGOR_Content* Sub : SubContent)
	{
		Sub->RemoveInsertion(Content);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_Content::BuildResource(UWorld* World)
{
	check(IsValid(World))
	AssignWorld(World);

	StorageSize = (Abstract ? 0 : 1);

	SINGLETON_CHK
}

void UTGOR_Content::PostBuildResource()
{
	if (Display.DisplayName.IsEmpty())
	{
		Display.DisplayName = FText::FromString(DefaultName);
	}

	// Add display name if no alias was defined
	if (ChatAlias.Num() == 0)
	{
		if (!Display.DisplayName.IsEmpty())
		{
			ChatAlias.Add(Display.DisplayName.ToString());
		}
		else
		{
			ChatAlias.Add(DefaultName);
		}
	}
}

void UTGOR_Content::BuildDefaultName(FString ModName)
{
	DefaultMod = ModName;
	DefaultName = GetClass()->GetName();
}

FString UTGOR_Content::GetDefaultName() const
{
	if (DefaultName.IsEmpty())
	{
		FString ClassName = GetClass()->GetName();
		ClassName.RemoveFromEnd("_C");
		return ClassName;
	}
	return DefaultName;
}

FString UTGOR_Content::GetModIdentifier() const
{
	if (IsValid(Mod))
	{
		Mod->Name + "-" + Mod->Version;
	}
	return "NoMod";
}

int32 UTGOR_Content::GetStorageIndex() const
{
	return StorageIndex;
}

const FTGOR_Display& UTGOR_Content::GetDisplay() const
{
	return(Display);
}

void UTGOR_Content::Reset()
{
	Insertions.Reset();
	SubContent.Reset();
	Singleton = nullptr;
}

bool UTGOR_Content::IsOfType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		return IsA(Type);
	}
	return false;
}

#ifdef WITH_EDITOR
void UTGOR_Content::SetDefaultName(const FString& Name)
{
	DefaultName = Name;
	MarkPackageDirty();
}


void UTGOR_Content::SetRawDisplayName(const FText& Name)
{
	Display.DisplayName = Name;
	MarkPackageDirty();
}

void UTGOR_Content::SetIsAbstract(bool IsAbstract)
{
	Abstract = IsAbstract;
	MarkPackageDirty();
}

void UTGOR_Content::AddInsertion(TSubclassOf<UTGOR_Content> Class)
{
	InsertionClasses.AddUnique(Class);
	MarkPackageDirty();
}

void UTGOR_Content::RemoveInsertion(TSubclassOf<UTGOR_Content> Class)
{
	InsertionClasses.Remove(Class);
	MarkPackageDirty();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Content::IsAbstract() const
{
	return(Abstract);
}

const TArray<FString>& UTGOR_Content::GetAliases() const
{
	return(ChatAlias);
}


FString UTGOR_Content::GetFirstAlias() const
{
	if (ChatAlias.Num() > 0)
	{
		return(ChatAlias[0]);
	}
	return(GetDefaultName());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Content::ForAllContent(const FContentDelegate& Func)
{
	ForAllContent([Func](UTGOR_Content* Content)->bool
	{
		Func.ExecuteIfBound(Content);
		return(true);
	});
}

void UTGOR_Content::ForAllContent(std::function<bool(UTGOR_Content*)> Func)
{
	Func(this);
	for (UTGOR_Content* Sub : SubContent)
	{
		Sub->ForAllContent(Func);
	}
}


bool UTGOR_Content::ContainsInsertion(UTGOR_Content* Content) const
{
	if (Content)
	{
		return Insertions.Contains(Content);
	}
	return(false);
}

bool UTGOR_Content::ContainsInsertionFromType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		for (UTGOR_Content* Content : Insertions)
		{
			if (IsValid(Content) && Content->IsA(Type))
			{
				return(true);
			}
		}
	}
	return(false);
}

UTGOR_Content* UTGOR_Content::GetInsertionFromType(TSubclassOf<UTGOR_Content> Type) const
{
	if (*Type)
	{
		for (UTGOR_Content* Content : Insertions)
		{
			if (IsValid(Content) && Content->IsA(Type))
			{
				return(Content);
			}
		}
	}
	return(nullptr);
}

TArray<UTGOR_Content*> UTGOR_Content::GetInsertionListFromType(TSubclassOf<UTGOR_Content> Type) const
{
	TArray<UTGOR_Content*> Contents;
	if (*Type)
	{
		for (UTGOR_Content* Content : Insertions)
		{
			if (IsValid(Content) && Content->IsA(Type))
			{
				Contents.Add(Content);
			}
		}
	}
	return(Contents);
}

UTGOR_Content* UTGOR_Content::GetContentStatic(const UObject* WorldContextObject, TSubclassOf<UTGOR_Content> Class)
{
	check(WorldContextObject)

	UWorld* World = WorldContextObject->GetWorld();
	check(World)

	UTGOR_Singleton* Singleton = UTGOR_GameInstance::EnsureSingletonFromWorld(World);
	if (Singleton)
	{
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		if (ContentManager)
		{
			return(ContentManager->GetContentFromType(Class));
		}
	}

	ERRET("Singleton/ContentManager not found", Error, nullptr)
}
