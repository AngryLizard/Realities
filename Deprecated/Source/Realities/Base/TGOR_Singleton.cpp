// The Gateway of Realities: Planes of Existence.


#include "TGOR_Singleton.h"

#include "Engine/ObjectLibrary.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/System/Data/TGOR_UserData.h"
#include "Realities/Base/System/Data/TGOR_ConfigData.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Mod/TGOR_Mod.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Engine/World.h"
#include "Engine/BlueprintGeneratedClass.h"


UTGOR_Singleton::UTGOR_Singleton()
:	Super()
{
	ModSetup.BaseMod = UTGOR_Mod::StaticClass();
}

#define EVERB(S) if(Verbose) EOUT(S)

TArray<UClass*> LoadPaths(UClass* Class, std::wstring Path, bool Plugin, bool Verbose, TArray<UClass*>& Classes)
{
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(Class, true, GIsEditor);
	ObjectLibrary->AddToRoot();
	std::wstring str = L"/Game/Mods";

	if (Plugin)
	{
		str.append(L"/Plugins");
	}
	else
	{
		str.append(L"/").append(Path);
	}

	ObjectLibrary->LoadBlueprintsFromPath(str.c_str());
	TArray<UBlueprintGeneratedClass *> ClassesArray;
	ObjectLibrary->GetObjects<UBlueprintGeneratedClass>(ClassesArray);

	for (int32 i = 0; i < ClassesArray.Num(); ++i)
	{
		UBlueprintGeneratedClass * BlueprintClass = ClassesArray[i];
		if (!BlueprintClass->GetName().Contains(FString("SKEL_"), ESearchCase::CaseSensitive))
		{
			Classes.Add(BlueprintClass);
		}
	}

	EVERB(FString::FromInt(Classes.Num()) + " objects found in " + FString(str.c_str()))

	return(Classes);
}

TArray<FTGOR_ContentLink> Load(UTGOR_Mod* Mod, bool Verbose)
{
	// Load paths to assets
	TArray<UClass*> Classes;
	LoadPaths(UTGOR_Content::StaticClass(), *Mod->Path, false, Verbose, Classes);

	TArray<FTGOR_ContentLink> Links;
	for (UClass* Class : Classes)
	{
		EVERB(Class->GetName() + " loaded.")
		if (Class->IsChildOf(UTGOR_Content::StaticClass()))
		{
			FTGOR_ContentLink Link;
			Link.Action = ETGOR_ModAction::Add;
			Link.Content = Class;

			Links.Add(Link);
		}
	}

	EVERB(FString::FromInt(Links.Num()) + " objects loaded from " + Mod->Name)
	return(Links);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ConfigData* UTGOR_Singleton::GetConfigData() const 
{
	return ConfigData; 
}

UTGOR_UserData* UTGOR_Singleton::GetUserData() const 
{ 
	return UserData; 
}

UTGOR_ContentManager* UTGOR_Singleton::GetContentManager() const 
{ 
	return ContentManager; 
}

UTGOR_WorldData* UTGOR_Singleton::GetWorldData() const 
{ 
	return WorldData; 
}

FTGOR_Time UTGOR_Singleton::GetGameTimestamp() const
{ 
	return GameTimestamp; 
}

FTGOR_Version UTGOR_Singleton::GetVersion() const
{
	return Version;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_Singleton* UTGOR_Singleton::Construct(UObject* Outer, const FTGOR_ModInstance& LocalSetup)
{
	// Construct Singleton
	UTGOR_Singleton* Singleton = NewObject<UTGOR_Singleton>(Outer, TEXT("Singleton"));
	
	Singleton->ConfigData = NewObject<UTGOR_ConfigData>(Singleton, TEXT("ConfigData"));
	Singleton->UserData = NewObject<UTGOR_UserData>(Singleton, TEXT("UserData"));
	Singleton->ContentManager = NewObject<UTGOR_ContentManager>(Singleton, TEXT("ContentManager"));
	Singleton->WorldData = NewObject<UTGOR_WorldData>(Singleton, TEXT("WorldData"));

	// Construct Basemod
	if (*LocalSetup.BaseMod == nullptr)
	{ ERRET("No basemod loaded", Fatal, Singleton) }

	Singleton->ModSetup = LocalSetup;
	return(Singleton);
}


void UTGOR_Singleton::LocalModSetup(UWorld* World, bool Verbose)
{
	if (!(*ModSetup.BaseMod))
	{ 
		ERROR("No basemod loaded",  Fatal) 
	}
		
	// Load plugins after basemod
	TArray<UClass*> Classes;
	Classes.Add(ModSetup.BaseMod);
	LoadPaths(UTGOR_Mod::StaticClass(), L"", true, Verbose, Classes);
	
	// Load local mods
	Version = FTGOR_Version();
	ModSetup.ActiveMods.Reset();
	for (UClass* Class : Classes)
	{
		if (Class->IsChildOf(UTGOR_Mod::StaticClass()))
		{
			UTGOR_Mod* Mod = Class->GetDefaultObject<UTGOR_Mod>();
			if (Mod == nullptr) continue;

			FTGOR_ModEntry Entry = { Mod->Name, Mod->Version, Mod->GetClass()->GetName() };
			Version.AddIdentifier(Mod->Name);
			ModSetup.ActiveMods.Add(Entry);
		}
	}

	LoadModSetup(World, ModSetup);
}


ETGOR_ModloadEnumeration UTGOR_Singleton::LoadModSetup(UWorld* World, const FTGOR_ModInstance& Setup)
{
	// Make sure base version matches
	if (ModSetup.BaseVersion != Setup.BaseVersion)
	{
		return(ETGOR_ModloadEnumeration::WrongVersion);
	}

	ContentManager->AssignWorld(World);
	ContentManager->Reset();
	
	// Load plugins
	TArray<UClass*> Classes;
	Classes.Add(ModSetup.BaseMod);
	LoadPaths(UTGOR_Mod::StaticClass(), L"", true, false, Classes);

	// Create mod classes
	TArray<UTGOR_Mod*> Mods;
	for (UClass* Class : Classes)
	{
		if (Class->IsChildOf(UTGOR_Mod::StaticClass()))
		{
			Mods.Add(NewObject<UTGOR_Mod>(this, Class));
		}
	}
	
	// Load server mods
	Version = FTGOR_Version();
	TArray<UTGOR_Mod*> Loaded;
	for (int i = 0; i < Setup.ActiveMods.Num(); i++)
	{
		const FTGOR_ModEntry& Entry = Setup.ActiveMods[i];

		bool Found = false;

		// Find mod with the same name in modlist
		for (UTGOR_Mod* Mod : Mods)
		{
			if (Mod->Name.Equals(Entry.Name) && !Found)
			{
				Version.AddIdentifier(Mod->Name);
				if (!Mod->Version.Equals(Entry.Version)) return(ETGOR_ModloadEnumeration::WrongVersion);
				if (!Mod->GetClass()->GetName().Equals(Entry.Class)) return(ETGOR_ModloadEnumeration::WrongMod);

				Mod->AssignWorld(Mod);
				TArray<FTGOR_ContentLink> Links = Load(Mod, false);
				ContentManager->ApplyMod(Mod, Links);
				Loaded.Add(Mod);
				Found = true;
			}
		}

		if(!Found)
		{
			return(ETGOR_ModloadEnumeration::NotFound);
		}
	}

	// Apply insertions
	ContentManager->ApplyInsertions();

	// Call post loaders
	for (UTGOR_Mod* Mod : Loaded)
	{
		Mod->PostAllModLoad(ContentManager->GetContentRoot());
	}

	ContentManager->PostLoad();

	return(ETGOR_ModloadEnumeration::Success);
}
